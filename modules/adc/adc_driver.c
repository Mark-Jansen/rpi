
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		// copy_[from/to]_user
//#include <linux/slab.h>			// kzalloc


#include "adc_internal.h"
#include "adc.h"


static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

static DEFINE_SPINLOCK(g_Lock);
static struct adc_config g_Config;

int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");


int adc_get_data(struct adc_data* arg)
{
	struct adc_config cfg;
	trace("");
	adc_get_config( &cfg );
	
	// read
	
	return -EINVAL;
}
EXPORT_SYMBOL(adc_get_data);

int adc_get_config(struct adc_config* arg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	arg->mode = g_Config.mode;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(adc_get_config);

int adc_set_config(struct adc_config* arg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	g_Config.mode = arg->mode;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(adc_set_config);


// file operations
static long adc_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct adc_data data;
	struct adc_config config;
	trace("");
	switch( command ) {
		case ADC_GET_DATA:
			if( copy_from_user( &data, (void*)arg, sizeof(struct adc_data) ) )
					return -EFAULT;
				ret = adc_get_data( &data );
			if( copy_to_user( (void*)arg, &data, sizeof(struct adc_data) ) )
				return -EFAULT;
			break;
		case ADC_GET_CONFIG:
			if( !adc_get_config( &config ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &config, sizeof(struct adc_config) ) )
				return -EFAULT;
			break;
		case ADC_SET_CONFIG:
			if( copy_from_user( &config, (void*)arg, sizeof(struct adc_config) ) )
				return -EFAULT;
			ret = adc_set_config( &config );
			break;
		default:
			break;
	}
	return ret;
}

static const struct file_operations adc_fops = {
	.owner				= THIS_MODULE,
	.unlocked_ioctl		= adc_ioctl,
};


//sysfs
static ssize_t show_status(struct device *dev, struct device_attribute *attr, char *buf)
{
	trace("");
	return snprintf(buf, PAGE_SIZE, "Status: 0\n");
}

static DEVICE_ATTR( status, S_IWUSR | S_IRUGO, show_status, NULL );


static int __init adc_init(void)
{
	int ret = -1;
	trace("");
	
	g_Major = register_chrdev( ADC_MAJOR, DRV_NAME, &adc_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = ADC_MAJOR;
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

	if( !ret ) {
		ret = adc_i2c_init();
	}

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

static void __exit adc_exit(void)
{
	trace("");
	
	adc_i2c_exit();
	device_remove_file( g_Device, &dev_attr_status );
	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	info("unloaded.");
}

module_init(adc_init);
module_exit(adc_exit);

MODULE_AUTHOR("Mark Jansen <mark@jansen.co.nl>");
MODULE_DESCRIPTION("ADC driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);