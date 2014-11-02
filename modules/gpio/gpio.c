
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user
#include <asm/io.h>
#include <mach/platform.h>

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
struct GpioRegisters *gpioRegister;

static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");

int gpio_set_config(struct gpio_status* arg)
{	
	int registerIndex;
	int oldRegister;
	
	// check for the right register.
	registerIndex = arg->pinNr / 10;
	// check old register settings
	oldRegister = gpioRegister->GPFSEL[registerIndex];			// oude register
	// change register settings
	int bit = (arg->pinNr % 10) * 3	;							// juist pin ivm 3 bits per pin voor functie
	int mask1 = 0b111 << bit;									// mask voor 3 bits van het juiste pinNr
	int mask2 = oldRegister & ~mask1;							// forceer de 3 bits van gekozen pinNr op 0
	int mask3 = (arg->value << bit) & mask1;					// mask voor het setten van de juiste waarde op pin nr
	gpioRegister->GPFSEL[registerIndex] = mask2 | mask3;		// geef gekozen pinNr juist waarde	

//	printk(KERN_INFO "Set function wiht GPFSEL. Register: %i\n", registerIndex);
//	printk(KERN_INFO "value = %i\n", gpioRegister->GPFSEL[registerIndex]);	
	int registerTest;
	registerTest = gpioRegister->GPFSEL[registerIndex];
	
	return 0;
}
EXPORT_SYMBOL(gpio_set_config);

int gpio_read(struct gpio_status* arg)
{
	int gpioPin = arg->pinNr;
	int inputvalue = arg->value;
	int gplevregister;
	int mask;
	int gplevValue;
	
	// check right register
	if ((gpioPin > 0) && (gpioPin < 54))	// GPIO Pin is available
	{
		if (gpioPin < 32)	// GPIO is at register 0
		{
			gplevregister = 0;
		}
		else
		{
			gplevregister = 1;
		}
		mask = 1 << gpioPin;	
		gplevValue = gpioRegister->GPLEV[gplevregister];
		arg->value = (gplevValue & mask) >> gpioPin;
	}
	else
	{
		printk(KERN_INFO "PinNr not available. \n");
	}
	
	return 0;
}
EXPORT_SYMBOL(gpio_read);

int gpio_write(struct gpio_status* arg)
{
	int gpioPin = arg->pinNr;
	int outputvalue = arg->value;
	int registerNr;
	int mask;
	int pinmask;
	
//	printk(KERN_INFO "In function gpio_write. Value = %i \n", outputvalue);
	
	// check right register
	if ((gpioPin > 0) && (gpioPin < 54))	// GPIO Pin is available
	{
		if (gpioPin < 32)	// GPIO is at register 0
		{
			registerNr = 0;
//			printk(KERN_INFO "gpset register 0 \n");
		}
		else
		{
			registerNr = 1;
//			printk(KERN_INFO "gpset register 1 \n");
			gpioPin = gpioPin - 32;		// start van volgende register
		}
	}
	else
	{
		printk(KERN_INFO "PinNr not available. \n");
	}
	// registerNr = gpioPin / 32;	// ipv if else hierboven
	// set outputvalue at right register
	mask = 1 << gpioPin;
	if (outputvalue)	// for set to 1 use GPSET
	{	
		int oldGPSETvalue = gpioRegister->GPSET[registerNr];	
		gpioRegister->GPSET[registerNr] = oldGPSETvalue | mask;
//		printk(KERN_INFO "write 1 with GPSET. Value = %i \n", outputvalue);
//		printk(KERN_INFO "value = %i\n", gpioRegister->GPSET);
	}
	else				// for set to 0 use GPCLR
	{
		int oldGPCLRvalue = gpioRegister->GPCLR[registerNr];
		gpioRegister->GPCLR[registerNr] = oldGPCLRvalue | mask;
//		printk(KERN_INFO "write 0 with GPCLR \n");
	}
	int	GPFSELregister = gpioRegister->GPFSEL[1];
	int gpfselmask = 0b111 << 24;
	int test = GPFSELregister & gpfselmask;
//	printk(KERN_INFO "gpfselregister for output pin = %i\n", test);
	return 0;
}
EXPORT_SYMBOL(gpio_write);

// file operations
static long gpio_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct gpio_status status;
  
	trace("");
	
	switch( command ) {		
		case GPIO_WRITE:
			if( copy_from_user( &status, (void*)arg, sizeof(struct gpio_status) ) )
				return -EFAULT;
			ret = gpio_write( &status );
			break;
		case GPIO_READ:
			if( copy_from_user( &status, (void*)arg, sizeof(struct gpio_status) ) )
				return -EFAULT;
			ret = gpio_read( &status );
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
	.open				= gpio_open,
	.release			= gpio_release,
	.unlocked_ioctl		= gpio_ioctl,
	.read				= gpio_read,
	.write				= gpio_write,
};


//sysfs
static ssize_t show_status(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct gpio_status status;
	status.pinNr = 2;
	trace("");
	if( !gpio_read( &status ) ) {
		return snprintf(buf, PAGE_SIZE, "Status: %i\n", status.pinNr);
	}
	return 0;
}

static ssize_t store_status(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct gpio_status status;
	trace("");
	if( sscanf(buf, "%d", &status.pinNr) == 1 ) {
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
	gpioRegister = (struct GpioRegisters *)__io_address(GPIO_BASE);	// zet gpioRegister op het goede addres

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
