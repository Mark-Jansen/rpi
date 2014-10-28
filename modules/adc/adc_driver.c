
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
static struct adc_config g_Config = {
	.resolution = ADC_RESOLUTION_14B,
	.gain = ADC_GAIN_1
};

int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");


int adc_get_data(struct adc_data* data)
{
	struct adc_config cfg;
	trace("");
	adc_get_config( &cfg );
	return adc_read_device( &cfg, data );
}
EXPORT_SYMBOL(adc_get_data);

int adc_get_config(struct adc_config* cfg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	cfg->resolution = g_Config.resolution;
	cfg->gain = g_Config.gain;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(adc_get_config);

int adc_set_config(struct adc_config* cfg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	g_Config.resolution = cfg->resolution;
	g_Config.gain = cfg->gain;
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
static ssize_t show_config(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct adc_config cfg;
	trace("");
	if( adc_get_config( &cfg ) )
		return snprintf( buf, PAGE_SIZE, "Failed to read the config\n" );
	return snprintf(buf, PAGE_SIZE, "Resolution : %db, Gain: %d\n", 12 + ((cfg.resolution & 0xc)>>1), cfg.gain + 1 );
}

static ssize_t show_chanx(struct device *dev, struct device_attribute *attr, char *buf, int chan)
{
	struct adc_data data;
	trace("");
	data.channel = chan;
	if( adc_get_data( &data ) )
		return snprintf( buf, PAGE_SIZE, "Failed to read the data\n" );
	return snprintf(buf, PAGE_SIZE, "Value: %d\n", data.value );
}

static ssize_t show_chan0(struct device *dev, struct device_attribute *attr, char *buf)
{
	return show_chanx( dev, attr, buf, 0 );
}

static ssize_t show_chan1(struct device *dev, struct device_attribute *attr, char *buf)
{
	return show_chanx( dev, attr, buf, 1 );
}


static DEVICE_ATTR( config, S_IWUSR | S_IRUGO, show_config, NULL );
static DEVICE_ATTR( chan0, S_IWUSR | S_IRUGO, show_chan0, NULL );
static DEVICE_ATTR( chan1, S_IWUSR | S_IRUGO, show_chan1, NULL );


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
	
	ret = device_create_file( g_Device, &dev_attr_config );
	ret |= device_create_file( g_Device, &dev_attr_chan0 );
	ret |= device_create_file( g_Device, &dev_attr_chan1 );

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
	device_remove_file( g_Device, &dev_attr_config );
	device_remove_file( g_Device, &dev_attr_chan0 );
	device_remove_file( g_Device, &dev_attr_chan1 );
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
