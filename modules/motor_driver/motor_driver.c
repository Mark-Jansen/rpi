
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user

#include <gpio.h>
#include <pwm.h>

#define DRV_NAME		"motor_driver"
#define DRV_REV			"r1"
#define EXAMPLE_MAJOR	250

#include "motor_driver.h"

#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;


static DEFINE_SPINLOCK(g_Lock);
static int g_Status = 0;


static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");

int motor_driver_get_config(struct motor_driver_config* arg)
{
	/*unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	arg->status = g_Status;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;*/
}
EXPORT_SYMBOL(setSpeed);

int setSpeed(struct motor_driver_setting* arg)
{
	/*unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	arg->status = g_Status;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;*/
}
EXPORT_SYMBOL(setSpeed);

int getSpeed(struct motor_driver_encoder_data* arg)
{
	/*
	unsigned long flags;
	trace("new value: %d", arg->status);
	spin_lock_irqsave( &g_Lock, flags );
	g_Status = arg->status;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;*/
}
EXPORT_SYMBOL(getSpeed);


// file operations
static long motor_driver_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct motor_driver_config mconfig;
	struct motor_driver_setting msetting;
	struct motor_driver_encoder_data mdata;
	trace("");
	switch( command ) {
		case MOTOR_DRIVER_SET_CONFIG:
			if( copy_from_user( &mconfig, (void*)arg, sizeof(struct motor_driver_config) ) )
				return -EFAULT;
			ret = setSpeed( &mconfig );
			break;
		case MOTOR_SETSPEED:
			if( !getSpeed( &msetting ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &msetting, sizeof(struct motor_driver_setting) ) )
				return -EFAULT;
			break;
		case MOTOR_GETSPEED:
			if( !getSpeed( &mdata ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &mdata, sizeof(struct motor_driver_encoder_data) ) )
				return -EFAULT;
			break;
		default:
			break;
	}
	return ret;
}

static int motor_driver_open(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}

static int motor_driver_release(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}

static const struct file_operations motor_driver_fops = {
	.owner				= THIS_MODULE,
	.open				= motor_driver_open,
	.release			= motor_driver_release,
	.unlocked_ioctl		= motor_driver_ioctl,
};


//sysfs
static ssize_t show_config(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct motor_driver_config mconfig;
	trace("");
	if( motor_driver_get_config( &mconfig ) ) {
		return snprintf( buf, PAGE_SIZE, "Failed to read the config\n" )	
	}else{
		return snprintf(buf, PAGE_SIZE, "pinnr: %db, leftpinnr: %db, rightpinnr: %db/n",mconfig.pinnr,mconfig.direction_left_pinnr,mconfig.direction_right_pinnr);
	}
	return 0;
}

static ssize_t show_pwm_setting(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct motor_driver_setting msetting;
	trace("");
	if( !setSpeed( &msetting ) ) {
		return snprintf(buf, PAGE_SIZE, "Setting: %i\n", msetting.pwm);
	}
	return 0;
}

static ssize_t show_direction_setting(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct motor_driver_setting msetting;
	trace("");
	if( !setSpeed( &msetting ) ) {
		return snprintf(buf, PAGE_SIZE, "Setting: %i\n", msetting.direction_pinL);
		return snprintf(buf, PAGE_SIZE, "Setting: %i\n", msetting.direction_pinR);
	}
	return 0;
}

static ssize_t show_motor_direction(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct motor_driver_encoder_data mdata;
	trace("");
	if( !getSpeed( &mdata ) ) {
		if(mdata.direction == 0)	
			return snprintf("Direction: LEFT");
		if(mdata.direction == 1)	
			return snprintf("Direction: RIGHT");
	}
	return 0;
}

static ssize_t show_motor_speed(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct motor_driver_encoder_data mdata;
	trace("");
	if( !getSpeed( &mdata ) ) {
			return snprintf(buf, PAGE_SIZE, "Setting: %i\n", mdata.speed);
	}
	return 0;
}

static DEVICE_ATTR( config, S_IWUSR | S_IRUGO, show_config, NULL );
static DEVICE_ATTR( pwm_pin, S_IWUSR | S_IRUGO, show_pwm_pin, NULL );
static DEVICE_ATTR( pwm_setting, S_IWUSR | S_IRUGO, show_pwm_setting, NULL );
static DEVICE_ATTR( direction_setting, S_IWUSR | S_IRUGO, show_direction_setting, NULL );
static DEVICE_ATTR( motor_direction, S_IWUSR | S_IRUGO, show_motor_direction, NULL );
static DEVICE_ATTR( motor_speed, S_IWUSR | S_IRUGO, show_motor_speed, NULL );

static int __init motor_driver_init(void)
{
	int ret = -1;
	trace("");
	
	g_Major = register_chrdev( MOTOR_DRIVER_MAJOR, DRV_NAME, &motor_driver_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = MOTOR_DRIVER_MAJOR;
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
	
	ret = device_create_file( g_Device, &dev_attr_pwm_pin );
	ret |= device_create_file( g_Device, &dev_attr_pwm_setting );
	ret |= device_create_file( g_Device, &dev_attr_direction_setting );
	ret |= device_create_file( g_Device, &dev_attr_motor_direction );
	ret |= device_create_file( g_Device, &dev_attr_motor_speed );

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

static void __exit motor_driver_exit(void)
{
	trace("");
	
	device_remove_file( g_Device, &dev_attr_pwm_pin );
	device_remove_file( g_Device, &dev_attr_pwm_setting );
	device_remove_file( g_Device, &dev_attr_direction_setting );
	device_remove_file( g_Device, &dev_attr_motor_direction );
	device_remove_file( g_Device, &dev_attr_motor_speed );
	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	info("unloaded.");
}

module_init(motor_driver_init);
module_exit(motor_driver_exit);

MODULE_AUTHOR("Hung Tran <hung@contronics.nl>");
MODULE_DESCRIPTION("Motor driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);
