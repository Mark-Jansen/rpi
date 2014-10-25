
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/slab.h>			// kzalloc

#include "adc_internal.h"
#include "adc.h"

#if !defined(CONFIG_I2C) && !defined(CONFIG_I2C_MODULE)
#error "I2C should be enabled in the kernel!"
#endif

static struct i2c_client *g_Client;
static DEFINE_SPINLOCK(g_I2CLock);

int adc_read_device(struct adc_config* cfg, struct adc_data* data)
{
	unsigned long flags;
	trace("");
	data->value = 0;
	spin_lock_irqsave( &g_I2CLock, flags );
	//
	spin_unlock_irqrestore( &g_I2CLock, flags );
	return -EINVAL;
}
EXPORT_SYMBOL(adc_read_device);

static int __devinit adc_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	trace("addr: 0x%x", client->addr);

	if( !i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA |
			I2C_FUNC_SMBUS_I2C_BLOCK) ) {
		error( "not all functionality supported" );
		return -ENODEV;
	}
	
	return 0;
}

static int __devexit adc_i2c_remove( struct i2c_client *client )
{
	trace("");
	return 0;
}

static struct i2c_board_info adc_i2c_board_info = {
	I2C_BOARD_INFO("MCP3422A0", 0x68),
	.platform_data = &adc_i2c_board_info,
};

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
	int busnum = 1;
	struct i2c_adapter *adap;
	struct i2c_client* client;
	unsigned long flags;
	
	trace("");

	adap = i2c_get_adapter(busnum);
	if (!adap) {
		error( "failed to get adapter i2c%d\n", busnum );
		return -ENODEV;
	}
	client = i2c_new_device(adap, &adc_i2c_board_info);
	spin_lock_irqsave( &g_I2CLock, flags );
	g_Client = client;
	spin_unlock_irqrestore( &g_I2CLock, flags );
	if( !client ) {
		error( "failed to register %s to i2c%d\n", adc_i2c_board_info.type, busnum);
	}

	return i2c_add_driver( &adc_i2c_driver );
}

void __devexit adc_i2c_exit()
{
	unsigned long flags;
	struct i2c_client* client;
	trace("");
	spin_lock_irqsave( &g_I2CLock, flags );
	client = g_Client;
	g_Client = NULL;
	spin_unlock_irqrestore( &g_I2CLock, flags );
	if( client ) {
		i2c_unregister_device( client );
	}
	i2c_del_driver( &adc_i2c_driver );
}

