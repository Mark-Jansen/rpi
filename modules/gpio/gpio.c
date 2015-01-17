// =================================================================================
// ====                                                                         ====
// ==== File      : gpio.c					                                    ====
// ====                                                                         ====
// ==== Function  : gpio_driver                   	                            ====
// ====                                                                         ====
// ==== Author    : Stefan van Nunen    			                            ====
// ====                                                                         ====
// ==== History   : Version 2.00                                                ====
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
#include <asm/io.h>
#include <asm/irq.h>
#include <mach/platform.h>
#include "gpio.h"
#include <linux/interrupt.h> 	// INTERRUPT TEST!!
#include <linux/gpio.h>


// =================================================================================
// ====   Defines	                                                            ====
// =================================================================================
#define DRV_NAME		"gpio"
#define DRV_REV			"r2"
#define DRV_MAJOR		0

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

 int gpio_irqs[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 				// 54 gpio pins default is -1
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0};													 
				
 int gpio_interrupt_counter[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 				// 54 gpio pins default is -1
								  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								  0, 0, 0, 0};					

// =================================================================================
// void wait_cycles()
// Pre : 
// Post: wait nr off cycles
// =================================================================================
void wait_cycles(int cycles)
{	
	int i;
	for (i=0; i<cycles; i++)     // wait cycles
	{
		udelay(1);
	}	
}

/******************************  START INTERRUPTS  ******************************/
// =================================================================================
// static irqreturn_t gpio_isr(int irq, void *data))
// Pre : 
// Post: interrupt routine called when interrupt
// =================================================================================
static irqreturn_t gpio_isr(int irq, void *data)
{
	int i = 0;	
	unsigned long flags;

	for(i=0; i<53; i++)
	{
		if(irq == gpio_irqs[i])	
		{
			// when there is an interrupt increment the interrupt counter
			spin_lock_irqsave( &g_Lock, flags );			
			gpio_interrupt_counter[i] += 1;
			udelay(10);	// for debouncing button
			spin_unlock_irqrestore( &g_Lock, flags );
		}	
	}
	return IRQ_HANDLED;
}


// =================================================================================
// void gpio_reset_all_interrupt_event(void)
// Pre : 
// Post: all interrupts are removed
// =================================================================================
void gpio_reset_all_interrupt_event(void)
{
	int i = 0;
	for(i=0; i<54; i++)
	{
		if(gpio_irqs[i] > 0)				// when there is an interrupt available
		{
			free_irq(gpio_irqs[i], NULL);	// free interrupt
			gpio_irqs[i] = 0;
		}
	}
}

// =================================================================================
// int gpio_set_event_detect(struct gpio_status* arg)
// Pre : 
// Post: sets gpio event detect
// Return: -1 when failed. 0 when successful
// =================================================================================
int gpio_set_event_detect(int pinnr, int event)
{
	int ret = 0;
		
	if(gpio_irqs[pinnr] > 0)				// when there was another interrupt
	{
		free_irq(gpio_irqs[pinnr], NULL);	// free old interrupt value
		gpio_irqs[pinnr] = 0;				// set irq struct to 0
		wait_cycles(150);					// wait a bit
	}
	
	if(event > 0)							// make new interrupt
	{
		// Request to IRQ
		ret = gpio_to_irq(pinnr);

		// check if request is succefull	
		if(ret < 0) 
		{
			error("Unable to request IRQ: %d", ret );
			return -1;
		}
		
		// set interrupt responding with pinnr in irq struct
		gpio_irqs[pinnr] = ret;

		// create an interrupt
		switch(event)
		{
			case RISING_EDGE_DETECT:			
				ret = request_irq(gpio_irqs[pinnr], gpio_isr, IRQF_TRIGGER_RISING | IRQF_DISABLED, "gpio_interrupt", NULL);
				break;
			case FALLING_EDGE_DETECT:
				ret = request_irq(gpio_irqs[pinnr], gpio_isr, IRQF_TRIGGER_FALLING | IRQF_DISABLED, "gpio_interrupt", NULL);
				break;	
			default:
				error("Set interrupt failed..	event = %d", event );
				gpio_irqs[pinnr] = 0;
				break;		
		}
	}

	return 0;
}
/******************************  END INTERRUPTS  ******************************/	

// =================================================================================
// int gpio_set_pull_up_down(struct gpio_status* arg)
// Pre : 
// Post: sets gpio pull up/down
// =================================================================================
int gpio_set_pull_up_down(int pinnr, int pud)
{	
	int registerIndex = pinnr/32;
	int pin = pinnr%32;
	unsigned long flags;	

	// when pud is a legal value (pull-up, pull-down, or pull-off)
	if( (PULL_DOWN_ENABLE >= pud) && (pud >= PULL_OFF) )
	{
		spin_lock_irqsave( &g_Lock, flags );
		gpioRegister->GPPUD = pud;							// set the required control signal	
		wait_cycles(150);	
		gpioRegister->GPPUDCLK[registerIndex] = 1 << pin;	// clock the control signal into the GPIO pads you wish to modify
		wait_cycles(150);
		gpioRegister->GPPUD = PULL_OFF;						// remove the required control signal	
		gpioRegister->GPPUDCLK[registerIndex] = 0 << pin;	// Remove clock
		spin_unlock_irqrestore( &g_Lock, flags );		
	}
	else
	{
		error("illegal pull up/down value. value = %d ", pud );
	}
	
	return 0;
}


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
		
	//=====	Set gpio function	======//
	registerIndex = arg->pinNr / 10;						// check for the right register.
	spin_lock_irqsave( &g_Lock, flags );	
	oldRegister = gpioRegister->GPFSEL[registerIndex];		// check old register
	// change register settings
	bit = (arg->pinNr % 10) * 3	;							// there are 10 gpiopins for 1 register. we need 3 bits to set the function of 1 gpiopin
	mask1 = 0b111 << bit;									// a mask to find the 3 bits for this gpiopin
	mask2 = oldRegister & ~mask1;							// a mask to set the 3 bits for this gpiopin to zero and save the other bits
	mask3 = (arg->function << bit) & mask1;					// a mask to set the 3 bits for this gpiopin to the correct value
	mask4 = mask2 | mask3;									// a mask to set the other bits back 
	gpioRegister->GPFSEL[registerIndex] = mask4;			// set the new value into the register 		
	spin_unlock_irqrestore( &g_Lock, flags );	
	
	//=====	Set pull up/down	======//
	gpio_set_pull_up_down(arg->pinNr, arg->pull_up_down);

	//=====	Set event detect	======//
	gpio_set_event_detect(arg->pinNr, arg->event_detect);

	//=====	Clear irqCount	======//
	arg->irqCount = 0;
	
	return 0;	// for debug
}
EXPORT_SYMBOL(gpio_set_config);

// =================================================================================
// int gpio_read(struct gpio_status* arg)
// Pre : 
// Post: When detect == NO_DETECT: 	the current pinvalue is set in arg->value
//		 When there is a detect: 	the number of detects is set in arg->value
// =================================================================================
int gpio_read(struct gpio_status* arg)
{
	int gpioPin = arg->pinNr%32;
	int readRegister = 0;
	int readRegister_2;
	int registerIndex;
	int pinMask;
	unsigned long flags;
	
	trace("");
	
	// check right register
	if ((gpioPin >= 0) && (gpioPin <= 53))	// GPIO Pin is available
	{
		registerIndex = gpioPin/32;			// find register for this gpioPin
		pinMask = 1 << gpioPin;				// make pinMask for the right pinNr
		
		// Read actual value with GPLEV
		spin_lock_irqsave( &g_Lock, flags );
		readRegister = gpioRegister->GPLEV[registerIndex];		// read the register
		spin_unlock_irqrestore( &g_Lock, flags );
		readRegister_2 = readRegister & pinMask;				// select only the correct pin 	
		arg->value = readRegister_2 >> gpioPin;					// Use shift for the value "0" or "1" 	
		arg->irqCount = gpio_interrupt_counter[arg->pinNr];	
		gpio_interrupt_counter[arg->pinNr] = 0;
	}
	else
	{
		error("PinNr not available. PinNr = %d ", gpioPin);
	}
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
	int pin = arg->pinNr;
	int outputvalue = arg->value;
	int registerNr = 0;
	int pinMask;
	unsigned long flags;

	trace("");
		
	// check right register
	if ((pin >= 0) && (pin <= 53))	// pin is available
	{
		registerNr = pin/32;		// find register for this pin
		pin = pin%32;				// find pin in register
	}
	else
	{
		error("PinNr not available. PinNr = %d ", pin);
		return -1;
	}

	// set outputvalue with the right register
	pinMask = 1 << pin;
	spin_lock_irqsave( &g_Lock, flags );
	if (outputvalue)	// for set to 1 we must use GPSET
	{	
		gpioRegister->GPSET[registerNr] = pinMask;	// Set pin to "1"
	}
	else				// for set to 0 we must use GPCLR
	{
		gpioRegister->GPCLR[registerNr] = pinMask;	// Set pin to "0" 
	}
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(gpio_write);



// =================================================================================
// int gpio_get_irqCount(struct gpio_status* arg)
// Pre : 
// Post: set interrupt counts in irqCount
// =================================================================================
int gpio_get_irqCount(struct gpio_status* arg)
{
	arg->irqCount = gpio_interrupt_counter[arg->pinNr];	// read interrupt counter
	gpio_interrupt_counter[arg->pinNr] = 0;				// clear interrupt counter
	
	return 0;
}
EXPORT_SYMBOL(gpio_get_irqCount);


// =================================================================================
// static long led_ioctl(struct file *file, unsigned int command, unsigned long arg)
// Pre : 
// Post: uses the command function
// =================================================================================
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
		case GPIO_GET_IRQCOUNT:
			if( copy_from_user( &status, (void*)arg, sizeof(struct gpio_status) ) )
				return -EFAULT;
			ret = gpio_get_irqCount( &status );
			if( copy_to_user( (void*)arg, &status, sizeof(struct gpio_status) ) )
				return -EFAULT;
			break;
		default:
			break;
	}
	return ret;
}


static const struct file_operations fops = {
	.owner				= THIS_MODULE,
	.unlocked_ioctl		= gpio_ioctl
};


//sysfs
static ssize_t read_gpio(struct device *dev, struct device_attribute *attr, char *buf)
{
	trace("");
	return snprintf(buf, PAGE_SIZE, "GPFSEL0: %d\n GPFSEL1: %d\n GPFSEL2: %d\n GPLEV0: %d\n GPLEV0: %d\n", (int)gpioRegister->GPFSEL[0], (int)gpioRegister->GPFSEL[1], (int)gpioRegister->GPFSEL[2], (int)gpioRegister->GPLEV[0], (int)gpioRegister->GPLEV[1]);
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
	
	gpio_reset_all_interrupt_event();				// free all intterupts
	
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