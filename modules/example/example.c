
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>			//kzalloc
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user

#define DRV_NAME		"example"
#define DRV_REV			"r1"
#define EXAMPLE_MAJOR	252

#include "example.h"


static struct platform_device *example_device;


// This is our private data that we 
struct example_data {
	int major;
	int minor;
	int open_count;
	struct cdev cdev;
};

static int g_Status = 0;

int example_get_status(struct example_status* arg)
{
	arg->status = g_Status;
	return 0;
}
EXPORT_SYMBOL(example_get_status);

int example_set_status(struct example_status* arg)
{
	g_Status = arg->status;
	printk(DRV_NAME ": status set to %d\n", g_Status);
	return 0;
}
EXPORT_SYMBOL(example_set_status);


// file operations
static long example_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct example_status status;
	switch( command ) {
		case EXAMPLE_SET_STATUS:
			if( copy_from_user( &status, (void*)arg, sizeof(struct example_status) ) )
				return -EFAULT;
			ret = example_set_status( &status );
			break;
		case EXAMPLE_GET_STATUS:
			if( !example_get_status( &status ) )
				ret = 0;
			else
				return -ENXIO;
			if( copy_to_user( (void*)arg, &status, sizeof(struct example_status) ) )
				return -EFAULT;
			break;
		default:
			break;
	}
	return ret;
}

static int example_open(struct inode *inode, struct file *file)
{
	struct example_data *data;
	data = container_of(inode->i_cdev, struct example_data, cdev);
	file->private_data = data;
	data->open_count++;
	return 0;
}

static int example_release(struct inode *inode, struct file *file)
{
	struct example_data *data = file->private_data;
	data->open_count--;
	return 0;
}

static const struct file_operations example_fops = {
	.owner				= THIS_MODULE,
	.open				= example_open,
	.release			= example_release,
	.unlocked_ioctl		= example_ioctl,
};


//sysfs
static ssize_t show_status(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct example_status status;
	if( !example_get_status( &status ) ) {
		return snprintf(buf, PAGE_SIZE, "Status: %i\n", status.status);
	}
	return 0;
}

static ssize_t store_status(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct example_status status;
	if( sscanf(buf, "%d", &status.status) == 1 ) {
		if( example_set_status( &status ) ) {
			printk( KERN_ERR DRV_NAME ": error setting status\n" );
		}
	} else {
		printk( KERN_ERR DRV_NAME ": error reading status\n" );
	}
	return count;
}

static DEVICE_ATTR(status, S_IWUSR | S_IRUGO, show_status, store_status);


// Character device setup
static int example_setup_dev(struct example_data *data, const struct file_operations *fops, int minor)
{
	int ret, devno;

	cdev_init(&data->cdev, fops);
	data->cdev.owner = THIS_MODULE;
	devno = MKDEV(data->major, minor);
	ret = cdev_add(&data->cdev, devno, 1);		//register a char device
	if (ret < 0) {
		printk(KERN_ERR DRV_NAME ": error %d registering device node %d, %d\n", ret, data->major, minor);
		goto out;
	}
	data->minor = minor;

out:
	return ret;
}


static int example_probe(struct platform_device *pdev)
{
	struct example_data *data;
	int ret;
	dev_t dev;

	// Allocate private data
	data = kzalloc(sizeof(struct example_data), GFP_KERNEL);
	if (!data) {
		ret = -ENOMEM;
		goto out;
	}

	// setup sysfs
	ret = device_create_file(&pdev->dev, &dev_attr_status);
	if (ret)
		goto out_free;

	// Setup character device
	data->major = EXAMPLE_MAJOR;
	if (data->major) {
		// if we have a fixed major number, use that
		dev = MKDEV(data->major, 0);
		ret = register_chrdev_region(dev, 1, DRV_NAME);
	} else {
		//if we do not have it, request one
		ret = alloc_chrdev_region(&dev, 0, 1, DRV_NAME);
		data->major = MAJOR(dev);
	}
	if (ret < 0)
		goto out_file;

	ret = example_setup_dev(data, &example_fops, 0);
	if (ret)
		goto out_dev;

	// Register private data for device
	platform_set_drvdata(pdev, data);
	printk(KERN_INFO DRV_NAME ": registered node %d, %d\n", data->major, data->minor);
	goto out;

out_dev:
	unregister_chrdev_region(MKDEV(data->major, 0), 1);
out_file:
	device_remove_file(&pdev->dev, &dev_attr_status);
out_free:
	kfree(data);
out:
	return ret;
}

static int example_remove(struct platform_device *pdev)
{
	struct example_data *data = platform_get_drvdata(pdev);

	cdev_del(&data->cdev);
	unregister_chrdev_region(MKDEV(data->major, 0), 1);
	
	device_remove_file(&pdev->dev, &dev_attr_status);

	kfree(data);
	return 0;
}



static struct platform_driver example_driver = {
	.probe = example_probe,
	.remove = example_remove,
	.driver = {
		.name = DRV_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init example_init(void)
{
	int ret;

	ret = platform_driver_register(&example_driver);
	if (ret)
		goto exit;

	example_device = platform_device_alloc(DRV_NAME, -1);
	if (!example_device) {
		ret = -ENOMEM;
		goto exit_driver;
	}

	ret = platform_device_add(example_device);
	if (ret)
		goto exit_device_put;

	printk(KERN_INFO DRV_NAME ": driver %s loaded\n", DRV_REV);

	goto exit;

exit_device_put:
	platform_device_put(example_device);
exit_driver:
	platform_driver_unregister(&example_driver);
exit:
	return ret;
}

static void __exit example_exit(void)
{
    platform_device_unregister(example_device);
    platform_driver_unregister(&example_driver);
    printk(KERN_INFO DRV_NAME ": unloaded.\n");
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("Mark Jansen <mark@jansen.co.nl>");
MODULE_DESCRIPTION("Example driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);

