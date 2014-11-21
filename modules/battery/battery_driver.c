
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user
#include <linux/timer.h>

#include "battery.h"
#include "../adc/adc.h"
#include "../common/common.h"

// Spinlock to protect the global charge and config state.
static DEFINE_SPINLOCK(g_Lock);
static struct battery_charge g_Charge;
static struct battery_config g_Config = {
	.resolution = ADC_RESOLUTION_14B,
	.max_value = 0,
	.num_samples = 3,
	.sample_interval = 10 * 1000
};
static struct timer_list g_Timer;
// we cannot use adc (i2c) from a timer, so we have to use a workqueue
static struct workqueue_struct* g_Workqueue;
static struct work_struct g_Work;


static void reschedule(int msec)
{
	trace( "in %d msec", msec );
	if( timer_pending( &g_Timer ) ) {
		del_timer( &g_Timer );
	}
	if( msec ) {
		mod_timer( &g_Timer, jiffies + msecs_to_jiffies(msec) );
	}
}


// retrieve the global battery charge state (available as exported symbol and ioctl)
int battery_get_charge(struct battery_charge* arg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	memcpy( arg, &g_Charge, sizeof( *arg ) );
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(battery_get_charge);

// retrieve the global config state (available as exported symbol and ioctl)
int battery_get_config(struct battery_config* cfg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	memcpy( cfg, &g_Config, sizeof( *cfg ) );
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(battery_get_config);

// set the global config state,
//  reschedule the timer if the interval is changed.
int battery_set_config(struct battery_config* cfg)
{
	unsigned long flags;
	int timeout = 0;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	if( g_Config.sample_interval != cfg->sample_interval ) {
		timeout = cfg->sample_interval;
	}
	memcpy( &g_Config, cfg, sizeof( g_Config ) );
	spin_unlock_irqrestore( &g_Lock, flags );
	if( timeout ) {
		reschedule( timeout );
	}
	return 0;
}
EXPORT_SYMBOL(battery_set_config);


// helper function to read the adc on the specified channel & resolution.
static int read_adc(int channel, int resolution, int* value)
{
	struct adc_config cfg = {
		.resolution = resolution,
		.gain = 0
	};
	struct adc_data data = {
		.channel = channel
	};
	trace("channel: %d", channel);
	data.channel = channel;
	if( adc_read_device( &cfg, &data ) )
		return -EIO;
	*value = data.value;
	return 0;
}

// scale the provided values (2) so that max = 100%
//  when max is not specified, queries the adc for the maximum value that can be reached
//  on the provided resolution
static void scale( int resolution, int max, int* values )
{
	int scalar = max;
	trace("");
	if( !scalar ) {
		scalar = adc_max_for_res( resolution );
	}
	values[0] = values[0] * 100 / scalar;
	values[1] = values[1] * 100 / scalar;
}

// workqueue callback, reads the adc on 2 channels, and converts the value to a percentage.
static void read_battery_work(struct work_struct *work)
{
	struct battery_config config;
	struct battery_charge charge;
	int i, chan, total[2], tmp;
	unsigned long flags;
	trace("");
	if( battery_get_config( &config ) )
		return;
	
	memset( &charge, 0, sizeof(charge) );
	total[0] = total[1] = 0;
	for( i = 0; i < config.num_samples; ++i ) {
		for( chan = 0; chan < 2; ++chan ) {
			if( read_adc( chan, config.resolution, &tmp ) )
				goto out_reschedule;
			total[chan] += tmp;
			if( i > 0 ) {
				total[chan] >>= 1;
			}
		}
	}	
	
	scale( config.resolution, config.max_value, total );
	trace("charge %d%%, %d%%", total[0], total[1]);

	spin_lock_irqsave( &g_Lock, flags );
	g_Charge.level1 = total[0];
	g_Charge.level2 = total[1];
	spin_unlock_irqrestore( &g_Lock, flags );

out_reschedule:
	reschedule( config.sample_interval );
}

// timer callback, schedules work so the adc will be read in a valid context
static void read_battery_fn(unsigned long arg)
{
	trace("");
	queue_work( g_Workqueue, &g_Work );
}

// file operations
static long battery_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct battery_charge charge;
	struct battery_config config;
	trace("");
	switch( command ) {
		case BATTERY_GET_CHARGE:
			if( !battery_get_charge( &charge ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &charge, sizeof(struct battery_charge) ) )
				return -EFAULT;
			break;
		case BATTERY_GET_CONFIG:
			if( !battery_get_config( &config ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &config, sizeof(struct battery_config) ) )
				return -EFAULT;
			break;
		case BATTERY_SET_CONFIG:
			if( copy_from_user( &config, (void*)arg, sizeof(struct battery_config) ) )
				return -EFAULT;
			ret = battery_set_config( &config );
			break;
		default:
			break;
	}
	return ret;
}

static const struct file_operations battery_fops = {
	.owner				= THIS_MODULE,
	.unlocked_ioctl		= battery_ioctl,
};


//sysfs
static ssize_t show_level(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct battery_charge charge;
	trace("");
	if( !battery_get_charge( &charge ) ) {
		return snprintf(buf, PAGE_SIZE, "Charge level: 1: %i%%, 2: %i%%\n", charge.level1, charge.level2);
	}
	return 0;
}

static DEVICE_ATTR( level, S_IWUSR | S_IRUGO, show_level, NULL );


static int battery_init(struct device* dev)
{
	int ret;
	trace("");

	ret = device_create_file( dev, &dev_attr_level );
	if( ret ) {
		error( "could not create file" );
		return ret;
	}
	g_Workqueue = create_singlethread_workqueue( "battery_workqueue" );
	if( g_Workqueue == NULL ) {
		device_remove_file( dev, &dev_attr_level );
		error( "could not create workqueue" );
		return -1;
	}
	INIT_WORK( &g_Work, read_battery_work );

	init_timer( &g_Timer );
	g_Timer.function = read_battery_fn;

	reschedule( 200 );		// schedule the first timer
	return ret;
}

static void battery_exit(struct device* dev)
{
	trace("");

	reschedule( 0 );		// make sure we kill the timer :)
	flush_workqueue( g_Workqueue );
	destroy_workqueue( g_Workqueue );
	device_remove_file( dev, &dev_attr_level );
}

struct driver_info info = {
	.name = "battery",
	.major = 250,
	.fops = &battery_fops,
	.init = battery_init,
	.exit = battery_exit
};


MODULE_AUTHOR("Mark Jansen <mark@jansen.co.nl>");
MODULE_DESCRIPTION("Battery charge indication driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("r2");
