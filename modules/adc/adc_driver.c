// adc global setup / entry symbols

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		// copy_[from/to]_user


#include "adc_internal.h"
#include "adc.h"
#include "../common/common.h"

// spinlock used to protect the global config
static DEFINE_SPINLOCK(g_Lock);
static struct adc_config g_Config = {
	.resolution = ADC_RESOLUTION_14B,
	.gain = ADC_GAIN_1
};


// read the adc with the global config, available as exported symbol and as ioctl
int adc_get_data(struct adc_data* data)
{
	struct adc_config cfg;
	trace("");
	adc_get_config( &cfg );
	return adc_read_device( &cfg, data );
}
EXPORT_SYMBOL(adc_get_data);

// read the global config, available as exported symbol and as ioctl
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

// store the global config, available as exported symbol and as ioctl
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

// helper function that returns maximum value for the specified adc resolution, available as exported symbol and as ioctl
int adc_max_for_res(int resolution)
{
	trace("");
	resolution &= 0xc;
	switch( resolution )
	{
	case ADC_RESOLUTION_12B:
		return ADC_RES_12B_MAX;
	case ADC_RESOLUTION_14B:
		return ADC_RES_14B_MAX;
	case ADC_RESOLUTION_16B:
		return ADC_RES_16B_MAX;
	case ADC_RESOLUTION_18B:
		return ADC_RES_18B_MAX;
	default:
		return 0;
	}
}
EXPORT_SYMBOL(adc_max_for_res);

// file operations
static long adc_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct adc_data data;
	struct adc_config config;
	int max_value;
	trace("");
	switch( command ) {
		case ADC_GET_DATA:
			if( copy_from_user( &data, (void*)arg, sizeof(data) ) )
					return -EFAULT;
				ret = adc_get_data( &data );
			if( copy_to_user( (void*)arg, &data, sizeof(data) ) )
				return -EFAULT;
			break;
		case ADC_GET_CONFIG:
			if( !adc_get_config( &config ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &config, sizeof(config) ) )
				return -EFAULT;
			break;
		case ADC_SET_CONFIG:
			if( copy_from_user( &config, (void*)arg, sizeof(config) ) )
				return -EFAULT;
			ret = adc_set_config( &config );
			break;
		case ADC_GET_MAX_FOR_RES:
			if( copy_from_user( &max_value, (void*)arg, sizeof(max_value) ) )
					return -EFAULT;
			max_value = adc_max_for_res( max_value );
			ret = 0;
			if( copy_to_user( (void*)arg, &max_value, sizeof(max_value) ) )
				return -EFAULT;
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


static int adc_init(struct device* dev)
{
	int ret;
	trace("");

	ret = device_create_file( dev, &dev_attr_config );
	ret |= device_create_file( dev, &dev_attr_chan0 );
	ret |= device_create_file( dev, &dev_attr_chan1 );

	if( !ret ) {
		ret = adc_i2c_init();
	}
	return ret;
}

static void adc_exit(struct device* dev)
{
	trace("");

	adc_i2c_exit();
	device_remove_file( dev, &dev_attr_config );
	device_remove_file( dev, &dev_attr_chan0 );
	device_remove_file( dev, &dev_attr_chan1 );
}

struct driver_info info = {
	.name = DRV_NAME,
	.major = 251,
	.fops = &adc_fops,
	.init = adc_init,
	.exit = adc_exit
};


MODULE_AUTHOR("Mark Jansen <mark@jansen.co.nl>");
MODULE_DESCRIPTION("ADC driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("r2");
