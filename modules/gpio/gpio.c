// =================================================================================
// ====                                                                         ====
// ==== File      : gpio.c					                                    ====
// ====                                                                         ====
// ==== Function  : gpio_driver                   	                            ====
// ====                                                                         ====
// ==== Author    : Stefan van Nunen    			                            ====
// ====                                                                         ====
// ==== History   : Version 1.00                                                ====
// ====             								                            ====
// ====                     							                        ====
// =================================================================================

// =================================================================================
// ====   I N C L U D E S                                                       ====
// =================================================================================
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user
#include <asm/io.h>
#include <mach/platform.h>


// =================================================================================
// ====   Defines	                                                            ====
// =================================================================================
#define DRV_NAME		"gpio"
#define DRV_REV			"r1"
#define DRV_MAJOR		0

#include "gpio.h"

#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )


// =================================================================================
// ====   S T R U C T S                                                         ====
// =================================================================================
static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

static DEFINE_SPINLOCK(g_Lock);
struct GpioRegisters *gpioRegister;

static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");


// =================================================================================
// int gpio_set_config(struct gpio_status* arg)
// Pre : 
// Post: sets gpio settings
// =================================================================================
int gpio_set_config(struct gpio_status* arg)
{	
	int registerIndex;
	int oldRegister;
	int bit;
	int mask1;
	int mask2;
	int mask3;
	int mask4;
	
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	
	// check for the right register.
	registerIndex = arg->pinNr / 10;
	// check old register settings
	oldRegister = gpioRegister->GPFSEL[registerIndex];			// oude register
	// change register settings
	bit = (arg->pinNr % 10) * 3	;							// juist pin ivm 3 bits per pin voor functie
	mask1 = 0b111 << bit;									// mask voor 3 bits van het juiste pinNr
	mask2 = oldRegister & ~mask1;							// forceer de 3 bits van gekozen pinNr op 0
	mask3 = (arg->function  << bit) & mask1;					// mask voor het setten van de juiste waarde op pin nr
	mask4 = mask2 | mask3;									// zet de overige bits weer in de oorspronkelijke stond terug
	gpioRegister->GPFSEL[registerIndex] = mask4;			// geef gekozen pinNr juist waarde	
		
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(gpio_set_config);

// =================================================================================
// int gpio_read(struct gpio_status* arg)
// Pre : 
// Post: read actual value of gpioPin
// =================================================================================
int gpio_read(struct gpio_status* arg)
{
	int gpioPin = arg->pinNr;
	int gplevregister;
	int mask;
	int gplevValue;
		
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	
	// check right register
	if ((gpioPin >= 0) && (gpioPin < 54))	// GPIO Pin is available
	{
		gplevregister = gpioPin/32;							// bepaal het goede register
		mask = 1 << gpioPin;								// mask voor de juiste gpio pin
		gplevValue = gpioRegister->GPLEV[gplevregister];	// lees het goed regiser uit
		arg->value = (gplevValue & mask) >> gpioPin;		// lees de goede gpio pin uit en shift hem zodat er alleen 1 of 0 uitkomt
	}
	else
	{
		printk(KERN_INFO "PinNr not available. \n");
	}
		
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(gpio_read);

// =================================================================================
// int gpio_write(struct gpio_status* arg)
// Pre : 
// Post: write arg->value to arg->pinNr
// =================================================================================
int gpio_write(struct gpio_status* arg)
{
	int gpioPin = arg->pinNr;
	int outputvalue = arg->value;
	int registerNr = 0;
	int mask;

	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
		
	// check right register
	if ((gpioPin > 0) && (gpioPin < 54))	// GPIO Pin is available
	{
		registerNr = gpioPin/32;			// bepaal het goede register
	}
	else
	{
		printk(KERN_INFO "PinNr not available. \n");
	}

	// set outputvalue at right register
	mask = 1 << gpioPin;
	if (outputvalue)	// for set to 1 use GPSET
	{	
		gpioRegister->GPSET[registerNr] = mask;	// zet de juiste gpio pin op 1 
	}
	else				// for set to 0 use GPCLR
	{
		gpioRegister->GPCLR[registerNr] = mask;	// zet de juiste gpio pin op 0
	}
	
	spin_unlock_irqrestore( &g_Lock, flags );
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
};


//sysfs
static ssize_t read_gpio(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct gpio_status status;
	int ret;
	trace("");
	if( sscanf(buf, "%d", &status.pinNr) > 1 ) {
		ret = ( gpio_read( &status  ));
		return snprintf(buf, PAGE_SIZE, "gpioPin %i  \t has value \t: %i\n",status.value, ret);		
	} else {
		error( "error pinnr not available\n" );
	}
	return 0;
}

static DEVICE_ATTR( status, S_IWUSR | S_IRUGO, read_gpio, NULL );

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
