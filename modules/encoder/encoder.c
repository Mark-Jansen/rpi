#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user
#include <asm/io.h>
#include <mach/platform.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <asm/irq.h>
#include "encoder.h"
#include "../gpio/gpio.h"


#define DRV_NAME			"encoder"
#define DRV_REV				"r1"
#define ENCODER_MAJOR	0


#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;
struct timespec time_spec1;	
struct timespec time_spec2;	
long time;
volatile int encoderPulseCount = 0;	
volatile int encoderPosition = 5;
static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");

struct gpio_status encoderpin1;
struct gpio_status encoderpin2;

// =================================================================================
// interrupt handler
// =================================================================================
static irqreturn_t isr_handler(int irq_id, void *data)
{ 
	encoderPulseCount++;
	gpio_read(&encoderpin2);
	if (encoderpin2.value == 1)
	{
		encoderPosition = 0;
	}  
	else
	{ 
		encoderPosition = 1;
	}
	return IRQ_HANDLED;
}
// =================================================================================
// free interrupt 
// =================================================================================
void gpio_reset_all_interrupt_event(int irq)
{
	free_irq(irq, NULL);	// free interrupt
}

int encoder_data_set_config(struct encoder_data* arg)
{
	int interruptID ;		  		// unique interrupt ID
	int ret = 0;
	// set encoder data
	trace("");
	encoderpin1.pinNr = arg->encoder1_pinnr;
	encoderpin1.function = INPUT;
	encoderpin1.pull_up_down = 	PULL_OFF;		   //PULL_DOWN_ENABLE, PULL_UP_ENABLE, PULL_OFF)
	//encoderpin1.event_detect = RISING_EDGE_DETECT;
	
	encoderpin2.pinNr = arg->encoder2_pinnr;
	encoderpin2.function = INPUT;
	encoderpin2.pull_up_down = 	PULL_OFF;		   //PULL_DOWN_ENABLE, PULL_UP_ENABLE, PULL_OFF)
	encoderpin2.event_detect = NO_DETECT;
	
	gpio_set_config(&encoderpin1);
	gpio_set_config(&encoderpin2);
	// Request to IRQ
	interruptID = gpio_to_irq(encoderpin1.pinNr);
	if(interruptID < 0) 
	{
		error("Unable to request IRQ: %d", interruptID );
		return -1;
	}
	ret = request_irq(interruptID, isr_handler, IRQF_TRIGGER_RISING | IRQF_DISABLED, "encoder interrupt", NULL);
	return 0;
}
EXPORT_SYMBOL(encoder_data_set_config);

int get_rotation_speed(struct encoder_data* arg)
{
	int pulsecount = encoderPulseCount;
	int rspeed = 0;
	getnstimeofday(&time_spec1);	// get current time
	while(!((encoderPulseCount - pulsecount) == 12))
	{
		// wait till 12x pulse is over
	}
	getnstimeofday(&time_spec2);	//get current time to measure
	time = time_spec2.tv_nsec - time_spec1.tv_nsec;	 // time for 1 rotation
	rspeed = 60000000/time; 
	arg->rotation_speed = rspeed;
	return 0;
}
EXPORT_SYMBOL(get_rotation_speed);

int get_direction(struct encoder_data* arg)
{
	arg->direction = encoderPosition;
	printk("encoderPosistion = %d\n", encoderPosition );
	printk("encoderPulsecount = %d\n", encoderPulseCount );
	
	return 0;
}
EXPORT_SYMBOL(get_direction);


// file operations
static long encoder_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct encoder_data edata;
	trace("");
	switch( command ) {
		case ENCODER_SET_CONFIG:
			if( copy_from_user( &edata, (void*)arg, sizeof(struct encoder_data) ) )
				return -EFAULT;
			ret = encoder_data_set_config( &edata );
			break;
		case ENCODER_SPEED:
			if( copy_from_user( &edata, (void*)arg, sizeof(struct encoder_data) ) )
				return -EFAULT;
			ret =  !get_rotation_speed( &edata );
			break;
		case ENCODER_DIRECTION:
			if( copy_from_user( &edata, (void*)arg, sizeof(struct encoder_data) ) )
				return -EFAULT;
			if( !get_direction( &edata ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &edata, sizeof(struct encoder_data) ) )
				return -EFAULT;
			break;
		default:
			break;
	}
	return ret;
}

static int encoder_open(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}

static int encoder_release(struct inode *inode, struct file *file)
{
	trace("");
	return 0;
}

static const struct file_operations encoder_fops = {
	.owner				= THIS_MODULE,
	.open				= encoder_open,
	.release			= encoder_release,
	.unlocked_ioctl		= encoder_ioctl,
};


//sysfs
static ssize_t show_data(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct encoder_data encdata;
	trace("");
	if( encoder_data_set_config( &encdata ) ) {
		return snprintf( buf, PAGE_SIZE, "Failed to read the config\n" );	
	}else{
		return snprintf(buf, PAGE_SIZE, "enc1pinnr: %i, enc2pinnr: %i/n",encdata.encoder1_pinnr,encdata.encoder2_pinnr);
	}
	return 0;
}

static DEVICE_ATTR( data, S_IWUSR | S_IRUGO, show_data, NULL );


static int __init encoder_init(void)
{
	int ret = -1;
	trace("");
	
	g_Major = register_chrdev( ENCODER_MAJOR, DRV_NAME, &encoder_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = ENCODER_MAJOR;
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
	
	ret = device_create_file( g_Device, &dev_attr_data );

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

static void __exit encoder_exit(void)
{
	trace("");
	
	device_remove_file( g_Device, &dev_attr_data );

	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	info("unloaded.");
}

module_init(encoder_init);
module_exit(encoder_exit);

MODULE_AUTHOR("Hung Tran <hung@contronics.nl>");
MODULE_DESCRIPTION("Encoder");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);
