
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/slab.h>			// kzalloc

#include "adc_internal.h"
#include "adc.h"

#if !defined(CONFIG_I2C) && !defined(CONFIG_I2C_MODULE)
#error "I2C should be enabled in the kernel!"
#endif

#if !defined(CONFIG_I2C_BOARDINFO)
#error "I2C boardinfo should be enabled in the kernel!"
#endif

static int __devinit adc_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct adc_device *dev;
	trace("");

	if( !i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA |
			I2C_FUNC_SMBUS_I2C_BLOCK) ) {
		error( "not all functionality supported" );
		return -ENODEV;
	}
	
	dev = kzalloc( sizeof(struct adc_device), GFP_KERNEL );
	if( dev == NULL ) {
		error( "out of memory" );
		return -ENOMEM;
	}

	dev->client = client;
	i2c_set_clientdata( client, dev );

	return 0;
}

static int __devexit adc_i2c_remove( struct i2c_client *client )
{
	struct adc_device *dev = i2c_get_clientdata( client );
	trace("");

	kfree(dev);
	return 0;
}

static struct i2c_board_info adc_i2c_board_info = {
	I2C_BOARD_INFO("MCP3422A0", 4),
	.platform_data = &adc_i2c_board_info,
};
static struct i2c_client *g_Client;

static const struct i2c_device_id adc_i2c_id[] = {
	{ "MCP3422A0", 0 },
	{ }
};

static struct i2c_driver adc_i2c_driver = {
	.probe = adc_i2c_probe,
	.remove = __devexit_p(adc_i2c_remove),
	.id_table = adc_i2c_id,
	.class = I2C_CLASS_HWMON,
	.driver = {
		.name = DRV_NAME,
	},
};

int __init adc_i2c_init()
{
	struct i2c_adapter *adap;

	int busnum = 1;
	trace("");

	adap = i2c_get_adapter(busnum);
	if (!adap) {
		error( "failed to get adapter i2c%d\n", busnum );
		return -ENODEV;
	}
	g_Client = i2c_new_device(adap, &adc_i2c_board_info);
	if (!g_Client) {
		error( "failed to register %s to i2c%d\n", adc_i2c_board_info.type, busnum);
	}

	//i2c_put_adapter(adap);

	return i2c_add_driver( &adc_i2c_driver );
}

void __devexit adc_i2c_exit()
{
	trace("");
	if( g_Client ) {
		i2c_unregister_device( g_Client );
		g_Client = NULL;
	}
	
	i2c_del_driver( &adc_i2c_driver );
}

