
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

#define debug(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;


static DEFINE_SPINLOCK(g_Lock);
static int g_Status = 0;


static int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");


int example_get_status(struct example_status* arg)
{
	unsigned long flags;
	debug("");
	spin_lock_irqsave( &g_Lock, flags );
	arg->status = g_Status;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(example_get_status);

int example_set_status(struct example_status* arg)
{
	unsigned long flags;
	debug("new value: %d", arg->status);
	spin_lock_irqsave( &g_Lock, flags );
	g_Status = arg->status;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(example_set_status);


// file operations
static long example_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct example_status status;
	debug("");
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
	debug("");
	return 0;
}

static int example_release(struct inode *inode, struct file *file)
{
	debug("");
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
	debug("");
	if( !example_get_status( &status ) ) {
		return snprintf(buf, PAGE_SIZE, "Status: %i\n", status.status);
	}
	return 0;
}

static ssize_t store_status(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct example_status status;
	debug("");
	if( sscanf(buf, "%d", &status.status) == 1 ) {
		if( example_set_status( &status ) ) {
			printk( KERN_ERR DRV_NAME ": error setting status\n" );
		}
	} else {
		printk( KERN_ERR DRV_NAME ": error reading status\n" );
	}
	return count;
}

static DEVICE_ATTR( status, S_IWUSR | S_IRUGO, show_status, store_status );


static int __init example_init(void)
{
	int ret = -1;
	debug("");
	
	g_Major = register_chrdev( EXAMPLE_MAJOR, DRV_NAME, &example_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = EXAMPLE_MAJOR;
	}

	g_Class = class_create( THIS_MODULE, DRV_NAME );
	if( IS_ERR(g_Class) ) {
		error( "could not register class %s", DRV_NAME );
		ret = PTR_ERR( g_Class );
		goto out_chrdev;
	}
	
	g_Device = device_create( g_Class, NULL, MKDEV(g_Major, 0), NULL, DRV_NAME "_device" );
	if( IS_ERR(g_Class) ) {
		error( "could not create %s_device", DRV_NAME );
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

static void __exit example_exit(void)
{
	debug("");
	
	device_remove_file( g_Device, &dev_attr_status );
	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

    info("unloaded.");
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("Mark Jansen <mark@jansen.co.nl>");
MODULE_DESCRIPTION("Example driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);

