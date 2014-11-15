
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user
#include <linux/timer.h>

#define DRV_NAME		"battery"
#define DRV_REV			"r1"
#define BATTERY_MAJOR	250

#include "battery.h"
#include "../adc/adc.h"

#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;


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


static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");

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


static int __init battery_init(void)
{
	int ret = -1;
	trace("");
	
	g_Major = register_chrdev( BATTERY_MAJOR, DRV_NAME, &battery_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = BATTERY_MAJOR;
	}

	g_Class = class_create( THIS_MODULE, DRV_NAME );
	if( IS_ERR(g_Class) ) {
		error( "could not register class %s", DRV_NAME );
		ret = PTR_ERR( g_Class );
		goto out_chrdev;
	}
	
	g_Device = device_create( g_Class, NULL, MKDEV(g_Major, 0), NULL, DRV_NAME );
	if( IS_ERR(g_Device) ) {
		error( "could not create device %s", DRV_NAME );
		ret = PTR_ERR( g_Device );
		goto out_device;
	}
	ret = device_create_file( g_Device, &dev_attr_level );
	if( ret ) {
		error( "could not create file" );
		goto out_file;
	}
	g_Workqueue = create_singlethread_workqueue( "battery_workqueue" );
	if( g_Workqueue == NULL ) {
		error( "could not create workqueue" );
		ret = -1;
		goto out_workqueue;
	}
	INIT_WORK( &g_Work, read_battery_work );

	init_timer( &g_Timer );
	g_Timer.function = read_battery_fn;

	info( DRV_REV " loaded, major: %d", g_Major );
	reschedule( 200 );		// schedule the first timer

	goto out;

out_workqueue:
	device_remove_file( g_Device, &dev_attr_level );
out_file:
	device_destroy( g_Class, MKDEV(g_Major, 0) );
out_device:
	class_unregister( g_Class );
	class_destroy( g_Class );
out_chrdev:
	unregister_chrdev(g_Major, DRV_NAME);
out:
	return ret;
}

static void __exit battery_exit(void)
{
	trace("");

	reschedule( 0 );		// make sure we kill the timer :)
	flush_workqueue( g_Workqueue );
	destroy_workqueue( g_Workqueue );
	device_remove_file( g_Device, &dev_attr_level );
	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	info("unloaded.");
}

module_init(battery_init);
module_exit(battery_exit);

MODULE_AUTHOR("Mark Jansen <mark@jansen.co.nl>");
MODULE_DESCRIPTION("Battery charge indication driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);