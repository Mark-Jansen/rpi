
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user

#define DRV_NAME		"pwm"
#define DRV_REV			"r1"
#define PWM_MAJOR	252

#include "pwm.h"

#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

static int g_Major = 0;
//static struct class* g_Class = NULL;
//static struct device* g_Device = NULL;


static DEFINE_SPINLOCK(g_Lock);
//static int g_Status = 0;

static struct pwm_config g_Config;
static struct pwm_settings g_Settings;


static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");


int pwm_get_config(struct pwm_config* arg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	arg->channel = g_Config.channel;
	arg->pin = g_Config.pin;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_get_config);

int pwm_set_config(struct pwm_config* arg)
{
	unsigned long flags;
	trace("new channel: %d", arg->channel);
	trace("new pin: %d", arg->pin);
	spin_lock_irqsave( &g_Lock, flags );
	g_Config.channel = arg->channel;
	g_Config.pin = arg->pin;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_set_config);



// file operations
static long pwm_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	//struct pwm_status status;
	struct pwm_config config;
	trace("");
	switch( command ) {
/*		case PWM_SET_STATUS:
			if( copy_from_user( &status, (void*)arg, sizeof(struct pwm_status) ) )
				return -EFAULT;
			ret = pwm_set_status( &status );
			break;
		case PWM_GET_STATUS:
			if( !pwm_get_status( &status ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &status, sizeof(struct pwm_status) ) )
				return -EFAULT;
			break;
*/		case PWM_SET_CONFIG:
			if( copy_from_user( &config, (void*)arg, sizeof(struct pwm_config) ) )
				return -EFAULT;
			ret = pwm_set_config( &config );
			break;
		case PWM_GET_CONFIG:
			if( !pwm_get_config( &config ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &config, sizeof(struct pwm_config) ) )
				return -EFAULT;
			break;
		//TEST
		case PWM_GET_ENABLED:
            return g_Settings.enabled;
            break;		
		case PWM_SET_ENABLED:
            g_Settings.enabled = arg;
			break;		
		case PWM_GET_FREQUENCY:
            return g_Settings.frequency;
            break;		
		case PWM_SET_FREQUENCY:
            g_Settings.frequency = arg;
			break;		
		case PWM_GET_DUTY_CYCLE:
            return g_Settings.duty_cycle;
            break;		
		case PWM_SET_DUTY_CYCLE:
            g_Settings.duty_cycle = arg;
			break;		
			
			
			
			
		default:
			break;
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

/*
//sysfs
static ssize_t show_status(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct pwm_status status;
	trace("");
	if( !pwm_get_status( &status ) ) {
		return snprintf(buf, PAGE_SIZE, "Status: %i\n", status.status);
	}
	return 0;
}

static ssize_t store_status(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct pwm_status status;
	trace("");
	if( sscanf(buf, "%d", &status.status) == 1 ) {
		if( pwm_set_status( &status ) ) {
			error( "error setting status\n" );
		}
	} else {
		error( "error reading status\n" );
	}
	return count;
}

static DEVICE_ATTR( status, S_IWUSR | S_IRUGO, show_status, store_status );
*/

static int __init pwm_init(void)
{
	int ret = -1;
	trace("");
	
    //test
	g_Settings.enabled = false;	
	g_Settings.frequency = 5000;
	g_Settings.duty_cycle = 50;
	
        
	g_Major = register_chrdev( PWM_MAJOR, DRV_NAME, &pwm_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = PWM_MAJOR;
	}
/*
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
	
	ret = device_create_file( g_Device, &dev_attr_status );

	info( DRV_REV " loaded, major: %d", g_Major );

	goto out;

out_device:
	class_unregister( g_Class );
	class_destroy( g_Class );
out_chrdev:
	unregister_chrdev(g_Major, DRV_NAME);
*/
out:
    //quick fix 
	ret = 0;
	return ret;
}

static void __exit pwm_exit(void)
{
	trace("");
	
	//device_remove_file( g_Device, &dev_attr_status );
	//device_destroy( g_Class, MKDEV(g_Major, 0) );
	//class_unregister( g_Class );
	//class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	info("unloaded.");
}

module_init(pwm_init);
module_exit(pwm_exit);

MODULE_AUTHOR("Frank Eggink <freggink@gmail.com>");
MODULE_DESCRIPTION("Pwm driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);
