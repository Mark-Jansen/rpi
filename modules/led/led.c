// =================================================================================
// ====                                                                         ====
// ==== File      : led.c					                                    ====
// ====                                                                         ====
// ==== Function  : led_driver                   	                            ====
// ====                                                                         ====
// ==== Author    : Stefan van Nunen    			                            ====
// ====                                                                         ====
// ==== History   : Version 1.00                                                ====
// ====             								                            ====
// ====                     							                        ====
// ====                                                                         ====
// =================================================================================

// =================================================================================
// ====   I N C L U D E S                                                       ====
// =================================================================================
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user
#include <linux/delay.h>
#include "led.h"
#include "../gpio/gpio.h"


// =================================================================================
// ====   Defines	                                                            ====
// =================================================================================
#define LED_NAME		"led"
#define LED_REV			"r1"
#define LED_MAJOR		0

#define trace(format, arg...) do { if( debug & 1 ) pr_info( LED_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( LED_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( LED_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( LED_NAME ": " format "\n", ## arg )


// =================================================================================
// ====   S T R U C T S                                                         ====
// =================================================================================
static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

struct gpio_status led;

static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");


// =================================================================================
// int led_set_config(struct led_status* status)
// Pre : 
// Post: sets gpiopin to output and new value
// =================================================================================
int led_set_config(struct led_status* status)
{
	int returnValue = 0;
	// make gpio_status struct and set values
	led.pinNr = status->pinNr;
	led.value = status->value;
	led.function = OUTPUT;

	trace("");
	
	// use gpio.c
	returnValue = gpio_set_config(&led);

	return returnValue;	
}
EXPORT_SYMBOL(led_set_config);


// =================================================================================
// int led_on(struct led_status* status)
// Pre : 
// Post: set led on
// =================================================================================
int led_on(struct led_status* status)
{
	int returnValue = 0;
	// make gpio_status struct and set values
	led.pinNr = status->pinNr;
	led.value = ON;
	led.function = OUTPUT;

	trace("");

	// use gpio.c
	returnValue = gpio_write(&led);

	return returnValue;
}
EXPORT_SYMBOL(led_on);


// =================================================================================
// int led_off(struct led_status* status)
// Pre : 
// Post: set led off
// =================================================================================
int led_off(struct led_status* status)
{
	int returnValue = 0;
	// make gpio_status struct and set values
	led.pinNr = status->pinNr;
	led.value = OFF;
	led.function = OUTPUT;

	trace("");

	// use gpio.c
	returnValue = gpio_write(&led);

	return returnValue;
}
EXPORT_SYMBOL(led_off);


// =================================================================================
// int led_blink(struct led_status* status)
// Pre : 
// Post: set led to blink with blinkTimer for frequency
// =================================================================================
int led_blink(struct led_status* status)
{
	int i = 0;
	int ledToggle = 0;
	int returnValue = 0;
		
	// make gpio_status struct and set values
	led.pinNr = status->pinNr;
	led.value = OFF;
	led.function = OUTPUT;

	trace("");
		
	for(i=0; i<10; i++);
	{
		// Here must be a delay/timer  // msleep(status->blinkTimer);
		ledToggle = ~ledToggle;
		led.value = ledToggle;
	
		// use gpio.c
		returnValue = gpio_write(&led);
	}
	
	return returnValue;	
}
EXPORT_SYMBOL(led_blink);


// =================================================================================
// static long led_ioctl(struct file *file, unsigned int command, unsigned long arg)
// Pre : 
// Post: uses the command function
// =================================================================================
static long led_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct led_status status;
	
	trace("");

	switch( command ) {
		case LED_SET_CONFIG:	
			if( copy_from_user( &status, (void*)arg, sizeof(struct led_status) ) )
				return -EFAULT;
			ret = led_set_config( &status);
			break;
		case LED_ON:
			if( copy_from_user( &status, (void*)arg, sizeof(struct led_status) ) )
				return -EFAULT;
			ret = led_on( &status);	
			break;
		case LED_OFF:	
			if( copy_from_user( &status, (void*)arg, sizeof(struct led_status) ) )
				return -EFAULT;
			ret = led_off( &status);
			break;
		case LED_BLINK:	
			if( copy_from_user( &status, (void*)arg, sizeof(struct led_status) ) )
				return -EFAULT;
			ret = led_blink( &status);
			break;
		default:
			break;
	}
	return ret;
}

static int led_open(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}


static const struct file_operations led_fops = {
	.owner				= THIS_MODULE,
	.open				= led_open,
	.release			= led_release,
	.unlocked_ioctl		= led_ioctl,
};


//sysfs
static ssize_t show_status(struct device *dev, struct device_attribute *attr, char *buf)
{
/*	struct led_status status;
	status.pinNr = 2;
	trace("");
	if( !gpio_read( &status ) ) {
		return snprintf(buf, PAGE_SIZE, "Status: %i\n", status.pinNr);
	}
*/	return 0;
}

static ssize_t store_status(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
/*	struct led_status status;
	trace("");
	if( sscanf(buf, "%d", &status.pinNr) == 1 ) {
		if( gpio_read( &status ) ) {
			error( "error setting status\n" );
		}
	} else {
		error( "error reading status\n" );
	}
*/	return count;
}

static DEVICE_ATTR( status, S_IWUSR | S_IRUGO, show_status, store_status );

static int __init led_init(void)
{
	int ret = -1;
	trace("");
	
	g_Major = register_chrdev( LED_MAJOR, LED_NAME, &led_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = LED_MAJOR;
	}

	g_Class = class_create( THIS_MODULE, LED_NAME );
	if( IS_ERR(g_Class) ) {
		error( "could not register class %s", LED_NAME );
		ret = PTR_ERR( g_Class );
		goto out_chrdev;
	}
	
	g_Device = device_create( g_Class, NULL, MKDEV(g_Major, 0), NULL, LED_NAME );
	if( IS_ERR(g_Class) ) {
		error( "could not create %s", LED_NAME );
		ret = PTR_ERR( g_Device );
		goto out_device;
	}
	
	ret = device_create_file( g_Device, &dev_attr_status );
	if(ret)
	{
		error("Could not create file");
	}

	info( LED_REV " loaded, major: %d", g_Major );

	goto out;

//out_file:
//	device destroy(g_Class, MKDEV(g_Major, 0));
out_device:
	class_unregister( g_Class );
	class_destroy( g_Class );
out_chrdev:
	unregister_chrdev(g_Major, LED_NAME);
out:
	return ret;
}

static void __exit led_exit(void)
{
	trace("");
	
	device_remove_file( g_Device, &dev_attr_status );
	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, LED_NAME );

	info("unloaded.");
}

module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("Stefan van Nunen");
MODULE_DESCRIPTION("led driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(LED_REV);
