// gyro global setup / entry symbols

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		// copy_[from/to]_user


#include "gyro.h"
#include "gyro_internal.h"
#include "../common/common.h"

// read the gyro, available as exported symbol and as ioctl
int gyro_get_data(struct gyro_data* data)
{
	trace("");
	return gyro_read_device( data );
}
EXPORT_SYMBOL(gyro_get_data);

// file operations
static long gyro_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct gyro_data data;
	trace("");
	switch( command ) {
		case GYRO_GET_DATA:
			ret = gyro_get_data( &data );
			if( copy_to_user( (void*)arg, &data, sizeof(data) ) )
				return -EFAULT;
			break;
		default:
			break;
	}
	return ret;
}

static const struct file_operations gyro_fops = {
	.owner				= THIS_MODULE,
	.unlocked_ioctl		= gyro_ioctl,
};

static ssize_t gyro_data_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct gyro_data data;
	int temp;
	trace("");
	if( gyro_get_data( &data ) )
		return snprintf( buf, PAGE_SIZE, "Failed to read the data\n" );
	temp = data.raw_temperature / 34 + 365;
	return snprintf( buf, PAGE_SIZE, "Accell: %d, %d, %d\nTemp(approx): %d.%d\nGyro: %d, %d, %d\n",
			data.accel_x, data.accel_y, data.accel_z, temp / 10, temp % 10,
			data.gyro_x, data.gyro_y, data.gyro_z );
}

static DEVICE_ATTR( data, S_IWUSR | S_IRUGO, gyro_data_show, NULL );

static int gyro_init(struct device* dev)
{
	int ret;
	trace("");

	ret = device_create_file( dev, &dev_attr_data );

	if( !ret ) {
		ret = gyro_i2c_init();
	}
	return ret;
}

static void gyro_exit(struct device* dev)
{
	trace("");

	gyro_i2c_exit();
	device_remove_file( dev, &dev_attr_data );
}

struct driver_info info = {
	.name = DRV_NAME,
	.major = 0,
	.fops = &gyro_fops,
	.init = gyro_init,
	.exit = gyro_exit
};


MODULE_AUTHOR("Mark Jansen <mark@jansen.co.nl>");
MODULE_DESCRIPTION("Gyro & accellero driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("r1");
