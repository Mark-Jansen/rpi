
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user

#include "pwm.h"
#include "pwm_internal.h"

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;


static DEFINE_SPINLOCK(g_Lock);

static struct pwm_settings g_Settings[NR_OF_CHANNELS];


int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");


int pwm_get_settings(struct pwm_settings* arg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	arg->channel    = g_Settings[arg->channel].channel;
	arg->pin        = g_Settings[arg->channel].pin;
	arg->frequency  = g_Settings[arg->channel].frequency;
	arg->duty_cycle = g_Settings[arg->channel].duty_cycle;
	arg->enabled    = g_Settings[arg->channel].enabled;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_get_settings);

int pwm_set_settings(struct pwm_settings* arg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	g_Settings[arg->channel].channel    = arg->channel;
	g_Settings[arg->channel].pin        = arg->pin;
	g_Settings[arg->channel].frequency  = arg->frequency;
	g_Settings[arg->channel].duty_cycle = arg->duty_cycle;
	g_Settings[arg->channel].enabled    = arg->enabled;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_set_settings);



// file operations
static long pwm_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct pwm_settings settings;
	trace("");
	switch( command ) {
		case PWM_SET_SETTINGS:
			if( copy_from_user( &settings, (void*)arg, sizeof(struct pwm_settings) ) )
				return -EFAULT;
			ret = pwm_set_settings( &settings );
			break;
		case PWM_GET_SETTINGS:
	        /* first copy current setting to determine channel!! */
     		if( copy_from_user( &settings, (void*)arg, sizeof(struct pwm_settings) ) )
				return -EFAULT;
			/*......................................................*/	
			if( !pwm_get_settings( &settings ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &settings, sizeof(struct pwm_settings) ) )
				return -EFAULT;
			break;
     	//TEST
		case PWM_GET_ENABLED:
            //return g_Settings.enabled;
            break;		
		case PWM_SET_ENABLED:
            //g_Settings.enabled = arg;
			break;		
		case PWM_GET_FREQUENCY:
            //return g_Settings.frequency;
            break;		
		case PWM_SET_FREQUENCY:
            //g_Settings.frequency = arg;
			break;		
		case PWM_GET_DUTY_CYCLE:
            //return g_Settings.duty_cycle;
            break;		
		case PWM_SET_DUTY_CYCLE:
            //g_Settings.duty_cycle = arg;
			break;		
		default:
			return ENOTTY;
	}
	return ret;
}


static int pwm_open(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}

static int pwm_release(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}


static const struct file_operations pwm_fops = {
	.owner				= THIS_MODULE,
	.open				= pwm_open,
	.release			= pwm_release,
	.unlocked_ioctl		= pwm_ioctl,
};



static ssize_t show_pwmx(struct device *dev, struct device_attribute *attr, char *buf, int chan)
{
	struct pwm_settings s;
	trace("");
	s.channel = chan;
	if( pwm_get_settings( &s ) ) 
	    return snprintf( buf, PAGE_SIZE, "Failed to read the data\n" );
	return snprintf(buf, PAGE_SIZE, "Channel: %d, pin: %d, enabled: %d, frequency: %d, duty_cycle: %d\n",
		            s.channel,                                                                                
					s.pin,
					s.enabled,
					s.frequency,
					s.duty_cycle);
						
}


static ssize_t show_pwm0(struct device *dev, struct device_attribute *attr, char *buf)
{
    return show_pwmx( dev, attr, buf, 0 );						
}

static ssize_t show_pwm1(struct device *dev, struct device_attribute *attr, char *buf)
{
    return show_pwmx( dev, attr, buf, 1 );					
}


static DEVICE_ATTR( pwm0, S_IWUSR | S_IRUGO, show_pwm0, NULL );
static DEVICE_ATTR( pwm1, S_IWUSR | S_IRUGO, show_pwm1, NULL );


static int __init pwm_init(void)
{
	int ret = -1;
	trace("");
        
	g_Major = register_chrdev( PWM_MAJOR, DRV_NAME, &pwm_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = PWM_MAJOR;
	}

	g_Class = class_create( THIS_MODULE, DRV_NAME );
	if( IS_ERR(g_Class) ) {
		error( "could not register class %s", DRV_NAME );
		ret = PTR_ERR( g_Class );
		goto out_chrdev;
	}
	
	g_Device = device_create( g_Class, NULL, MKDEV(g_Major, 0), NULL, DRV_NAME );
	if( IS_ERR(g_Class) ) {
		error( "could not create %s", DRV_NAME );
		ret = PTR_ERR( g_Device );
		goto out_device;
	}
	
	ret = device_create_file( g_Device, &dev_attr_pwm0 );
    ret |= device_create_file( g_Device, &dev_attr_pwm1 );
	
	info( DRV_REV " loaded, major: %d", g_Major );

	goto out;

out_device:
	class_unregister( g_Class );
	class_destroy( g_Class );
out_chrdev:
	unregister_chrdev(g_Major, DRV_NAME);

out:

	return ret;
}

static void __exit pwm_exit(void)
{
	trace("");
	
	device_remove_file( g_Device, &dev_attr_pwm0 );
	device_remove_file( g_Device, &dev_attr_pwm1 );
	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	info("unloaded.");
}

module_init(pwm_init);
module_exit(pwm_exit);

MODULE_AUTHOR("Frank Eggink <freggink@gmail.com>");
MODULE_DESCRIPTION("Pwm driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);
