#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>

#include "common.h"

static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");

static int common_init(void)
{
	int ret = -1;
	trace("");
	
	ret = register_chrdev( info.major, info.name, info.fops );
	if( ret < 0 ) {
		error( "could not register device: %d", ret );
		goto out;
	} else if( ret != 0 ) {
		info.major = ret;
	}

	g_Class = class_create( THIS_MODULE, info.name );
	if( IS_ERR(g_Class) ) {
		error( "could not register class %s", info.name );
		ret = PTR_ERR( g_Class );
		goto out_chrdev;
	}
	
	g_Device = device_create( g_Class, NULL, MKDEV(info.major, 0), NULL, info.name );
	if( IS_ERR(g_Device) ) {
		error( "could not create %s", info.name );
		ret = PTR_ERR( g_Device );
		goto out_device;
	}

	if( info.init ) {
		ret = info.init( g_Device );
		if( ret ) {
			goto out_device;
		}
	}
	//ret = device_create_file( g_Device, &dev_attr_level );

	info( "loaded, major: %d", info.major );
	
	goto out;

out_device:
	class_unregister( g_Class );
	class_destroy( g_Class );
out_chrdev:
	unregister_chrdev( info.major, info.name );
out:
	return ret;
}

static void common_exit(void)
{
	trace("");
	
	if( info.exit ) {
		info.exit( g_Device );
	}
	
	//device_remove_file( g_Device, &dev_attr_level );
	device_destroy( g_Class, MKDEV(info.major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( info.major, info.name );

	info("unloaded.");
}

module_init(common_init);
module_exit(common_exit);
