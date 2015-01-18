#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
//#include <linux/delay.h> 
//#include <linux/hrtimer.h>
//#include <linux/sched.h>
#include <linux/uaccess.h>		//copy_[from/to]_user
#include <asm/io.h>
#include <mach/platform.h>

#include "motor_driver.h"
#include "../gpio/gpio.h"
#include "../pwm/pwm.h"
#include "../encoder/encoder.h"

#define DRV_NAME			"motor_driver"
#define DRV_REV				"r1"
#define MOTOR_DRIVER_MAJOR	0



#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");

static struct motor_driver_setting md_settings;
struct gpio_status IN1_pin;
struct gpio_status IN2_pin;
struct pwm_settings pwm_setting;
struct encoder_data enc_data;
	
int motor_driver_set_settings(struct motor_driver_setting* arg)
{
	trace("");
	IN1_pin.pinNr = arg->direction_in1_pinnr;
	IN1_pin.function = OUTPUT;
	IN1_pin.value = arg->direction_pinL;
	
	IN2_pin.pinNr = arg->direction_in2_pinnr;
	IN2_pin.function = OUTPUT;
	IN2_pin.value = arg->direction_pinR;
		
	pwm_setting.channel = arg->pwm_channel;
	pwm_setting.pin = arg->pwm_pinnr;
	pwm_setting.enabled =arg->pwm_enable;
	pwm_setting.frequency = arg->pwm_frequency;
	pwm_setting.duty_cycle = arg->pwm_duty_cycle;
	
	gpio_set_config(&IN1_pin);
	gpio_set_config(&IN2_pin);
	
	gpio_write(&IN1_pin);
	gpio_write(&IN2_pin);
	pwm_set_settings(&pwm_setting);
	return 0;
}
EXPORT_SYMBOL(motor_driver_set_settings);

int motor_driver_get_settings(struct motor_driver_setting* arg)
{
	trace("");
	arg->direction_in1_pinnr = md_settings.direction_in1_pinnr;
	arg->direction_in2_pinnr = md_settings.direction_in2_pinnr;
	arg->direction_pinL = md_settings.direction_pinL;
	arg->direction_pinR = md_settings.direction_pinR;
	arg->pwm_channel = md_settings.pwm_channel;
	arg->pwm_pinnr = md_settings.pwm_pinnr;
	arg->pwm_enable = md_settings.pwm_enable;
	arg->pwm_frequency = md_settings.pwm_frequency;
	arg->pwm_duty_cycle = md_settings.pwm_duty_cycle;
	arg->speed = md_settings.speed;
	arg->direction = md_settings.direction;
	return 0;
}
EXPORT_SYMBOL(motor_driver_get_settings);

int setSpeed(struct motor_driver_setting* arg)
{
	int ch = arg->pwm_channel;
	int dc = arg->pwm_duty_cycle;
	trace("");	
	if ((pwm_set_duty_cycle(ch,dc))== 0){
	return 0;
	}else{
	return -1;
	}
}
EXPORT_SYMBOL(setSpeed);

int getSpeed(struct motor_driver_setting* arg)
{	
	
	return 0;
}
EXPORT_SYMBOL(getSpeed);

int setDirection(struct motor_driver_setting* arg) // 0 == left 1 == right
{
	trace("");
	return 0;
}
EXPORT_SYMBOL(setDirection);


int getDirection(struct motor_driver_setting* arg)
{
	get_direction(&enc_data);
	arg->direction = enc_data.direction;
	return 0;
}
EXPORT_SYMBOL(getDirection);


// file operations
static long motor_driver_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct motor_driver_setting msetting;
	trace("");
	switch( command ) {
		case MOTOR_DRIVER_SET_SETTINGS:
			if( copy_from_user( &msetting, (void*)arg, sizeof(struct motor_driver_setting) ) )
				return -EFAULT;
			ret = motor_driver_set_settings( &msetting );
			break;
		case MOTOR_DRIVER_GET_SETTINGS:
			if( copy_from_user( &msetting, (void*)arg, sizeof(struct motor_driver_setting) ) )
				return -EFAULT;
			if( !motor_driver_get_settings( &msetting ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &msetting, sizeof(struct motor_driver_setting) ) )
				return -EFAULT;
			break;
		case MOTOR_DRIVER_SET_SPEED:
			if( copy_from_user( &msetting, (void*)arg, sizeof(struct motor_driver_setting) ) )
				return -EFAULT;
			ret =  !setSpeed( &msetting );
			break;
		case MOTOR_DRIVER_GET_SPEED:
			if( copy_from_user( &msetting, (void*)arg, sizeof(struct motor_driver_setting) ) )
				return -EFAULT;
			if( !getSpeed( &msetting ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &msetting, sizeof(struct motor_driver_setting) ) )
				return -EFAULT;
			break;
		case MOTOR_DRIVER_SET_DIRECTION:
			if( copy_from_user( &msetting, (void*)arg, sizeof(struct motor_driver_setting) ) )
				return -EFAULT;
			ret =  !setDirection( &msetting );
			break;
		case MOTOR_DRIVER_GET_DIRECTION:
			if( copy_from_user( &msetting, (void*)arg, sizeof(struct motor_driver_setting) ) )
				return -EFAULT;
			if( !getDirection( &msetting ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &msetting, sizeof(struct motor_driver_setting) ) )
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
	struct motor_driver_setting mconfig;
	trace("");
	if( motor_driver_get_settings( &mconfig ) ) {
		return snprintf( buf, PAGE_SIZE, "Failed to read the config\n" );	
	}else{
		return snprintf(buf, PAGE_SIZE, "pinnr: %db, leftpinnr: %db, rightpinnr: %db/n",mconfig.direction_in1_pinnr,mconfig.direction_in2_pinnr,mconfig.pwm_pinnr);
	}
	return 0;
}

static ssize_t show_pwm_setting(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct motor_driver_setting msetting;
	trace("");
	if( !motor_driver_get_settings( &msetting ) ) {
		return snprintf(buf, PAGE_SIZE, "Setting: %i\n", msetting.pwm_duty_cycle);
	}
	return 0;
}

static ssize_t show_direction_setting(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct motor_driver_setting msetting;
	trace("");
	if( !motor_driver_get_settings( &msetting ) ) {
		return snprintf(buf, PAGE_SIZE, "Setting: %i\n", msetting.direction_pinL);
		return snprintf(buf, PAGE_SIZE, "Setting: %i\n", msetting.direction_pinR);
	}
	return 0;
}

static ssize_t show_motor_direction(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct motor_driver_setting mdata;
	trace("");
	if( !motor_driver_get_settings( &mdata ) ) {
		if(mdata.direction_pinL == 0 && mdata.direction_pinR == 1)	
			return printk("Direction: LEFT");
		if(mdata.direction_pinL == 1 && mdata.direction_pinR == 0)	
			return printk("Direction: RIGHT");
	}
	return 0;
}

static ssize_t show_motor_speed(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct motor_driver_setting mdata;
	trace("");
	if( !motor_driver_get_settings( &mdata ) ) {
			return snprintf(buf, PAGE_SIZE, "Setting: %i\n", mdata.pwm_duty_cycle);
	}
	return 0;
}

static DEVICE_ATTR( config, S_IWUSR | S_IRUGO, show_config, NULL );
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
	
	ret = device_create_file( g_Device, &dev_attr_config );
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
	
	device_remove_file( g_Device, &dev_attr_config );
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
