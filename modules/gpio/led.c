
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user

#define DRV_NAME		"led"
#define DRV_REV			"r1"
#define DRV_MAJOR		252

#include "led.h"
#include "gpio.h"

#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

static DEFINE_SPINLOCK(g_Lock);

static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");



int led_on(int channel)
{
	// make gpio_status struct and set values
	struct gpio_status* led;
	led->channel = channel;
	led->value = LED_ON;
	led->function = OUTPUT;
	
	// use gpio function
	return = gpio_write(&led);
}
EXPORT_SYMBOL(led_on);


int led_off(int channel)
{
	// make gpio_status struct and set values
	struct gpio_status* led;
	led->channel = channel;
	led->value = LED_OFF;
	led->function = OUTPUT;
	
	// use gpio function
	return = gpio_write(&led);
}
EXPORT_SYMBOL(led_off);


// file operations
static long led_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct led_status status;
	trace("");
	switch( command ) {
		case LED_ON:
			if( copy_from_user( &status, (void*)arg, sizeof(struct led_status) ) )
				return -EFAULT;
			ret = led_on(status->channel);
			break;
		case LED_OFF:
			if( copy_from_user( &status, (void*)arg, sizeof(struct led_status) ) )
				return -EFAULT;
			ret = led_off(status->channel);
			break;
		default:
			break;
	}
	return ret;
}

static int led_open(status->channel)
{
	trace("");
	return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}

static const struct file_operations fops = {
	.owner				= THIS_MODULE,
	.unlocked_ioctl		= led_ioctl,
};


//sysfs
static ssize_t show_status(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct led_status status;
	trace("");
	if( !led_get_status( &status ) ) {
		return snprintf(buf, PAGE_SIZE, "Status: %i\n", status.status);
	}
	return 0;
}

static ssize_t store_status(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct led_status status;
	trace("");
	if( sscanf(buf, "%d", &status.status) == 1 ) {
		if( led_set_status( &status ) ) {
			error( "error setting status\n" );
		}
	} else {
		error( "error reading status\n" );
	}
	return count;
}

static DEVICE_ATTR( status, S_IWUSR | S_IRUGO, show_status, store_status );


static int __init led_init(void)
{
	int ret = -1;
	trace("");
	
	g_Major = register_chrdev( DRV_MAJOR, DRV_NAME, &fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = DRV_MAJOR;
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
	
	ret = device_create_file( g_Device, &dev_attr_status );

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

static void __exit led_exit(void)
{
	trace("");
	
	device_remove_file( g_Device, &dev_attr_status );
	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	info("unloaded.");
}

module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("Stefan van Nunen");
MODULE_DESCRIPTION("led driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);
