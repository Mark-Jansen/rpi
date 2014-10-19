
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user

#define DRV_NAME		"gpio"
#define DRV_REV			"r1"
#define DRV_MAJOR		252

#include "gpio.h"

#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

static DEFINE_SPINLOCK(g_Lock);
struct GpioRegisters gpioRegister;	// dit is het gpio register dat goed gezet moet worden via de datasheet gegevens.

static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");

int gpio_read(struct gpio_status* arg)
{
	int gpioPin = arg->channel;
	int inputvalue = arg->value;
	int gplevregister;
	int mask;

	// hier denk ik het GpioRegisters goed zetten
	gpioRegister.GPFSEL[arg->channel] = INPUT;	// input // nog controleren hoe het precies moet met mask enz. maar iets van deze manier
	arg->value = gpioRegister.GPFSEL[arg->channel];	// nog controleren hoe het precies moet met mask enz. maar iets van deze manier
	
	// check right register
	if ((gpioPin > 0) && (gpioPin > 54))	// GPIO Pin is available
	{
		if (gpioPin < 32)	// GPIO is at register 0
		{
			gplevregister = gpioRegister.GPLEV[0];
		}
		else
		{
			gplevregister = gpioRegister.GPLEV[1];
		}
		mask = 1 << gpioPin;		
		inputvalue = gplevregister & mask;
		return inputvale;
	}
	
	return 0;
}
EXPORT_SYMBOL(gpio_read);

int gpio_write(struct gpio_status* arg)
{
	int gpioPin = arg->channel;
	int outputvalue = arg->value;
	int gpsetregister;
	int mask
	
	// check right register
	if ((gpioPin > 0) && (gpioPin > 54)))	// GPIO Pin is available
	{
		if (gpioPin < 32)	// GPIO is at register 0
		{
			gpsetregister = 0;
		}
		else
		{
			gpsetregister = 1;
		}
	}
	
	// set outputvalue at right register
	mask = 1 << gpioPin;
	if (outputvalue)	// for set to 1 use GPSET
	{	
		int oldGPSETvalue = gpioRegister.GPSET[gpsetregister];	
		gpioRegister.GPSET[gpsetregister] = oldGPSETvalue & mask;
	}
	else				// for set to 0 use GPCLR
	{
		int oldGPCLRvalue = gpioRegister.GPCLR[gpsetregister];
		gpioRegister.GPSET[gpsetregister] = oldGPCLRvalue & mask;
	}
	
	return 0;
}
EXPORT_SYMBOL(gpio_write);

int gpio_set_config(struct gpio_status* arg)
{	
	int bit = arg->channel;
	int mask;
	int registerIndex;
	int oldRegister;
	
	// check for the right function
	if (arg->function == 1)
	{
		mask = OUTPUT << bit;									// mask voor het wijzigen van de juiste bits
	}
	else
	{
		mask = INPUT << bit;									// mask voor het wijzigen van de juiste bits
	}
	// check for the right register.
	registerIndex = arg->channel / 10;
	// check old register settings
	oldRegister = gpioRegister.GPFSEL[registerIndex];		// oude register
	// change register settings
	gpioRegister.GPFSEL[registerIndex] = oldRegister & (mask);	// oude register masken voor 0 te forceren
	gpioRegister.GPFSEL[registerIndex] = oldRegister | (mask);	// oude register masken voor 1 te forceren
	
	return 0;
}
EXPORT_SYMBOL(gpio_set_config);

// file operations
static long gpio_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct gpio_status status;
	trace("");
	switch( command ) {
		case GPIO_READ:
			if( copy_from_user( &status, (void*)arg, sizeof(struct gpio_status) ) )
				return -EFAULT;
			ret = gpio_read( &status );
			break;
		case GPIO_WRITE:
			if( !gpio_write( &status ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &status, sizeof(struct gpio_status) ) )
				return -EFAULT;
			break;
		case GPIO_SET_CONFIG:
			if( copy_from_user( &status, (void*)arg, sizeof(struct gpio_status) ) )
				return -EFAULT;
			ret = gpio_set_config( &status );
			break;
		default:
			break;
	}
	return ret;
}

static int gpio_open(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}

static int gpio_release(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}

static const struct file_operations fops = {
	.owner				= THIS_MODULE,
//	.open				= gpio_open,
//	.release			= gpio_release,
	.unlocked_ioctl		= gpio_ioctl,
//	.read				= gpio_read,
//	.write				= gpio_write,
};


//sysfs
static ssize_t show_status(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct gpio_status status;
	status.channel = 2;
	trace("");
	if( !gpio_read( &status ) ) {
		return snprintf(buf, PAGE_SIZE, "Status: %i\n", status.channel);
	}
	return 0;
}

static ssize_t store_status(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct gpio_status status;
	trace("");
	if( sscanf(buf, "%d", &status.channel) == 1 ) {
		if( gpio_read( &status ) ) {
			error( "error setting status\n" );
		}
	} else {
		error( "error reading status\n" );
	}
	return count;
}

static DEVICE_ATTR( status, S_IWUSR | S_IRUGO, show_status, store_status );


static int __init gpio_init(void)
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

static void __exit gpio_exit(void)
{
	trace("");
	
	device_remove_file( g_Device, &dev_attr_status );
	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	info("unloaded.");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_AUTHOR("Stefan van Nunen");
MODULE_DESCRIPTION("GPIO driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);
