#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/time.h>
#include "../gpio/gpio.h"
#include "ultrasoon.h"

#define DRV_NAME			"ultrasoon"
#define DRV_REV				"r3"
#define ultrasoon_MAJOR			 0

#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

// spinlock used to protect the global config
static DEFINE_SPINLOCK(g_Lock);
static struct ultrasoon_config g_Config = {
	.pinNr_Trigger_1 = 0,
	.pinNr_Trigger_2 = 0,
	.pinNr_echo_1 = 0,
	.pinNr_echo_2 = 0
};


struct gpio_status trigger_port;
struct gpio_status echo_port;

static int debug = 1;

module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");

int ultrasoon_set_config(struct ultrasoon_config* cfg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	if(cfg->pinNr_Trigger_1 != 0)
	{
		g_Config.pinNr_Trigger_1 = cfg->pinNr_Trigger_1;
		g_Config.pinNr_echo_2 = cfg->pinNr_echo_2;
		if(debug)
			printk(KERN_INFO "Config set for Sensor 1: T=%d, E=%d\n",cfg->pinNr_Trigger_1,cfg->pinNr_echo_1);	
	}
	
	if(cfg->pinNr_Trigger_2 != 0)
	{
		g_Config.pinNr_Trigger_2 = cfg->pinNr_Trigger_2;
		g_Config.pinNr_echo_2 = cfg->pinNr_echo_2;
		if(debug)
			printk(KERN_INFO "Config set for Sensor 2: T=%d, E=%d\n",cfg->pinNr_Trigger_2,cfg->pinNr_echo_2);	
	}

	spin_unlock_irqrestore( &g_Lock, flags );
	
	return 0;
}
EXPORT_SYMBOL(ultrasoon_set_config);


int ultrasoon_get_config(struct ultrasoon_config* cfg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	cfg->pinNr_Trigger_1 = g_Config.pinNr_Trigger_1;
	cfg->pinNr_Trigger_2 = g_Config.pinNr_Trigger_2;
	cfg->pinNr_echo_1 = g_Config.pinNr_echo_1;
	cfg->pinNr_echo_2 = g_Config.pinNr_echo_2;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(ultrasoon_get_config);

int ultrasoon_measure_distance(struct ultrasoon_config* cfg,struct ultrasoon_data* result)
{
	unsigned long GPIO_TIMEOUT_SEC 	= 200; //== 200ms // 0 == OFF // More 18.5 == useless because of the max timeout
	long int time_difference		= 0;
	
	struct timespec timeout_start	= {0};
	struct timespec start_timeVal   = {0};
	struct timespec end_timeVal	 	= {0};
	struct timespec delta_timeVal 	= {0};

	
	trigger_port.value 		= LOW;
	trigger_port.function  	= OUTPUT;
	result->distance 		= -1;
		
	//SET GPIO VOLGENS DE CFG CONFIG
	if(result->type == FRONT_SENSOR)
	{
		trigger_port.pinNr	= cfg->pinNr_Trigger_1;
		echo_port.pinNr		= cfg->pinNr_echo_1;
	}
	else
	{
		trigger_port.pinNr	= cfg->pinNr_Trigger_2;
		echo_port.pinNr	= cfg->pinNr_echo_2;
	}
	
	echo_port.function	= INPUT;
	echo_port.value 	= LOW;
	
	gpio_set_config(&trigger_port);
	gpio_set_config(&echo_port);
	
//##########################	DEBUG DATA		##########################
if(debug)
{
	printk(KERN_INFO "Current CONFIG:\nTrigger_port: %d \n",trigger_port.pinNr);
	printk(KERN_INFO "EchoPort: %d \n",echo_port.pinNr);
}
//##########################	DEBUG DATA		##########################
	
	if(gpio_write(&trigger_port) != 0)// set trigger port LOW
		printk(KERN_INFO "ERROR from GPIO_WRITE Line: %d \n",__LINE__);
		
	//SET SLEEP FOR 1 sec to let the sensor settel
	//msleep(1000);	
	
	//Set timmer timeout value
	if( GPIO_TIMEOUT_SEC > 0)
	{
		GPIO_TIMEOUT_SEC = GPIO_TIMEOUT_SEC * 1000000;
		getnstimeofday(&timeout_start);// = current_kernel_time();
		timeout_start.tv_nsec += GPIO_TIMEOUT_SEC;
		
		if( timeout_start.tv_nsec >= 1000000000)//indien nieuwe seconde .
		{							 
			timeout_start.tv_nsec -= 1000000000;
			timeout_start.tv_sec++;
		}
	}
	
//##########################	DEBUG DATA		##########################
if(debug && GPIO_TIMEOUT_SEC > 0)
{
	start_timeVal = current_kernel_time();
	printk(KERN_INFO "Current time: %lu\n",start_timeVal.tv_nsec);	
	printk(KERN_INFO "Timeout time: %lu\n",timeout_start.tv_nsec);	
}
//##########################	DEBUG DATA		##########################
	
	//Send the puls to Ultrasoon device
	trigger_port.value = HIGH;
	if(gpio_write(&trigger_port) != 0)
		printk(KERN_INFO "ERROR from GPIO_WRITE Line: %d \n",__LINE__);
					
	//msleep(0.0001);
	udelay(250); 
	trigger_port.value = LOW;
	
	if(gpio_write(&trigger_port) != 0)
		printk(KERN_INFO "ERROR from GPIO_WRITE Line: %d \n",__LINE__);
	
	//Wait till echoport get HIGH
	while(echo_port.value == 0)
	{
		if(gpio_read(&echo_port))
		{
			printk(KERN_INFO "ERROR from GPIO_WRITE Line: %d \n",__LINE__);
		}
			
		//start_timeVal = current_kernel_time();
		getnstimeofday(&start_timeVal);
				
		//TBV timeout
		if(start_timeVal.tv_nsec > timeout_start.tv_nsec && 
			start_timeVal.tv_sec >= timeout_start.tv_sec &&	
			GPIO_TIMEOUT_SEC > 0 )
		{
			printk(KERN_INFO "TIME OUT!, Echo did not go HIGH\n");//@ Line: %d \nstart_timeVal was: %lu\n",__LINE__,start_timeVal.tv_nsec);
			//printk(KERN_INFO "Timeout time was: %lu",timeout_start.tv_nsec);
			return 0;
		}
	}
	//count the time echo port is HIGH	
	while(echo_port.value != 0)
	{
		if(gpio_read(&echo_port))
		{
			printk(KERN_INFO "ERROR from GPIO_WRITE Line: %d \n",__LINE__);
		}
			
		//end_timeVal = current_kernel_time();
		getnstimeofday(&end_timeVal);
		
		//TBV timeout
		if(end_timeVal.tv_nsec > timeout_start.tv_nsec &&
			end_timeVal.tv_sec >= timeout_start.tv_sec &&
			GPIO_TIMEOUT_SEC > 0)
		{
			printk(KERN_INFO "TIME OUT!, echo did not go LOW\n");// OUT @ Line: %d / start_timeVal was: %lu\n",__LINE__,start_timeVal.tv_nsec);
			//printk(KERN_INFO "Timeout time is: %lu",timeout_start.tv_nsec);
			return 0;
		}
	}

	delta_timeVal = timespec_sub(end_timeVal,start_timeVal);
	time_difference = delta_timeVal.tv_sec * 100000000;
	time_difference += delta_timeVal.tv_nsec;
	
	//time_difference = end_timeVal.tv_nsec - start_timeVal.tv_nsec;
	
//##########################	DEBUG DATA		##########################
if(debug)
{
	printk(KERN_INFO "start_timeVal value: %lu\n",start_timeVal.tv_nsec);
	printk(KERN_INFO "end_timeVal value: %lu\n",end_timeVal.tv_nsec);
	printk(KERN_INFO "time_difference value: %lu\n",time_difference);
	
	if(start_timeVal.tv_nsec > end_timeVal.tv_nsec)
	{
		printk(KERN_INFO "WHAAAAAAAAAAAAAAAAA THIS MAY NEVER HAPPEN!!!!\n");
	}
}
//##########################	DEBUG DATA		##########################

	time_difference = time_difference / 1000;	//Turn nano into micro sec
	time_difference = time_difference * 17150;	// Speed of Sound 340.29 /2 because bounce back and 000 
	result->distance = time_difference / 1000000; 		//Round up into cm
	
//##########################	DEBUG DATA		##########################
if(debug)
	printk(KERN_INFO "distance value: %d\n",result->distance);
//##########################	DEBUG DATA		##########################
	
	return 0;
}

int ultrasoon_get_distance(struct ultrasoon_data* arg)
{
	struct ultrasoon_config cfg;
	trace("");
	ultrasoon_get_config(&cfg);
	return ultrasoon_measure_distance( &cfg, arg );
}
EXPORT_SYMBOL(ultrasoon_get_distance);


// file operations
static long ultrasoon_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct ultrasoon_data data;
	struct ultrasoon_config config;
	trace("");
	
	switch( command ) {
		case ULTRASOON_GET_DISTANCE:
			if( copy_from_user( &data, (void*)arg, sizeof(struct ultrasoon_data) ) )
				return -EFAULT;
			if( !ultrasoon_get_distance( &data ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &data, sizeof(struct ultrasoon_data) ) )
				return -EFAULT;
			break;
			
		case ULTRASOON_GET_CONFIG:
			if( !ultrasoon_get_config( &config ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &config, sizeof(struct ultrasoon_config) ) )
				return -EFAULT;
			break;
			
		case ULTRASOON_SET_CONFIG:
			if( copy_from_user( &config, (void*)arg, sizeof(struct ultrasoon_config) ) )
				return -EFAULT;
			ret = ultrasoon_set_config( &config );
			break;
		default:
			break;
	}
	return ret;
}

static const struct file_operations ultrasoon_fops = {
	.owner				= THIS_MODULE,
	.unlocked_ioctl		= ultrasoon_ioctl,
};


//sysfs
static ssize_t show_config(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct ultrasoon_config cfg;
	trace("");
	if( ultrasoon_get_config( &cfg ) )
		return snprintf( buf, PAGE_SIZE, "Failed to read the config\n" );
	return snprintf(buf, PAGE_SIZE, "Triger port1: %d \nEcho_1: %d\nEcho_2: %d\n",cfg.pinNr_Trigger_1, cfg.pinNr_echo_1,cfg.pinNr_echo_2 );
}

static ssize_t show_distance(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct ultrasoon_data cfg;
	trace("");
	if( ultrasoon_get_distance( &cfg ) )
		return snprintf( buf, PAGE_SIZE, "Failed to read the data\n" );
	return snprintf(buf, PAGE_SIZE, "Distance : %d CM, sensor Nr:%d\n",cfg.distance, cfg.type);
}

static DEVICE_ATTR( distance, S_IWUSR | S_IRUGO, show_distance, NULL );
static DEVICE_ATTR( config, S_IWUSR | S_IRUGO, show_config, NULL );

static int __init ultrasoon_init(void)
{
	int ret = -1;
	trace("");
	
	g_Major = register_chrdev( ultrasoon_MAJOR, DRV_NAME, &ultrasoon_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = ultrasoon_MAJOR;
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
	
	ret = device_create_file( g_Device, &dev_attr_distance );
	ret |= device_create_file( g_Device, &dev_attr_config );
	
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

static void __exit ultrasoon_exit(void)
{
	trace("");

	device_remove_file( g_Device, &dev_attr_config  );
	device_remove_file( g_Device, &dev_attr_distance  );

	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	info("unloaded.");
}



module_init(ultrasoon_init);
module_exit(ultrasoon_exit);

MODULE_AUTHOR("Nick de Waard");
MODULE_DESCRIPTION("ULTRASOON driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);

