// gyro i2c functions

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/slab.h>			// kzalloc
#include <linux/delay.h>

#include "gyro.h"
#include "gyro_internal.h"
#include "gyro_registers.h"
#include "../common/common.h"

// guard against a misconfigured kernel
#if !defined(CONFIG_I2C) && !defined(CONFIG_I2C_MODULE)
#error "I2C should be enabled in the kernel!"
#endif

#define GYRO_ADDRESS		0x69

// i2c client & mutex to protect it, we cannot use a spinlock here, this causes problems with the i2c functions.
static struct i2c_client *g_Client;
static DEFINE_MUTEX(g_I2CLock);


// helper function to write to the i2c device
static int i2c_write( char* buf, int count )
{
	int ret = -ENODEV;
	trace("");
	mutex_lock( &g_I2CLock );
	if( g_Client ) {
		ret = i2c_master_send( g_Client, buf, count );
	}
	mutex_unlock( &g_I2CLock );
	return (count == ret) ? 0 : ret;
}

// helper function to read from the i2c device
static int i2c_read( unsigned char address, char* buf, int count )
{
	int ret = -ENODEV;
	struct i2c_msg msg[2];
	trace("");
	mutex_lock( &g_I2CLock );
	if( g_Client ) {
		msg[0].addr = msg[1].addr = GYRO_ADDRESS;
		msg[0].flags = 0;
		msg[0].len = 1;
		msg[0].buf = &address;
		msg[1].flags = I2C_M_RD;
		msg[1].len = count;
		msg[1].buf = buf;
		ret = i2c_transfer( g_Client->adapter, msg, 2 );
	}
	mutex_unlock( &g_I2CLock );
	return (2 == ret) ? 0 : ret;
}

// read the gyro chip with i2c.
int gyro_read_device(struct gyro_data* data)
{
	char buf[14];
	trace("");
	if( i2c_read( REG_ACCEL_XOUT_H, buf, 14 ) ) {
		error("read failed");
		return -EIO;
	}
	data->accel_x = buf[0] << 8 | buf[1];
	data->accel_y = buf[2] << 8 | buf[3];
	data->accel_z = buf[4] << 8 | buf[5];
	data->raw_temperature = buf[6] << 8 | buf[7];
	data->gyro_x = buf[8] << 8 | buf[9];
	data->gyro_y = buf[10] << 8 | buf[11];
	data->gyro_z = buf[12] << 8 | buf[13];
	return 0;
}

// i2c setup code
static int gyro_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	trace("addr: 0x%x", client->addr);
	if( !i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA) ) {
		error( "not all functionality supported" );
		return -ENODEV;
	}
	return 0;
}

static int gyro_i2c_remove( struct i2c_client *client )
{
	trace("");
	return 0;
}

// define the i2c address the chip is registered on.
static struct i2c_board_info gyro_i2c_board_info = {
	I2C_BOARD_INFO("MPU6050", GYRO_ADDRESS),
	.platform_data = &gyro_i2c_board_info,
};

static const struct i2c_device_id gyro_i2c_id[] = {
	{ "MPU6050", 0 },
	{ }
};

static struct i2c_driver gyro_i2c_driver = {
	.probe = gyro_i2c_probe,
	.remove = __devexit_p(gyro_i2c_remove),
	.id_table = gyro_i2c_id,
	.class = I2C_CLASS_HWMON,
	.driver = {
		.name = DRV_NAME,
	},
};

static int gyro_write_setting(unsigned char address, unsigned char value)
{
	unsigned char buf[2] = { address, value };
	return i2c_write( buf, 2 );
}

static int gyro_init_settings(void)
{
	char buf[1];
	int ret = 0;
	if( i2c_read( REG_WHO_AM_I, buf, 1 ) ) {
		error("reading identifier failed");
		return -EIO;
	}
	if( buf[0] != 0x68 ) {
		error("another chip responds on my address!");
		return -EIO;
	}
	
	// reset to defaults, disable sleep.
	ret |= gyro_write_setting( REG_PWR_MGMT_1, 0x00 );
	
	// Sample rate to 1000 / (1 + 1) = 500Hz
	ret |= gyro_write_setting( REG_SMPLRT_DIV, 0x01 );
	// disable FSync, 44Hz DLPF
	ret |= gyro_write_setting( REG_CONFIG, 0x03 );
	// disable gyro self tests, scale of 500 degrees/s
	ret |= gyro_write_setting( REG_GYRO_CONFIG, 0x08 );
	// disable accel self tests, scale of +-4g, no DHPF
	ret |= gyro_write_setting( REG_ACCEL_CONFIG, 0x08 );

	if( ret ) {
		error("writing defaults");
		return -EIO;
	}
	return 0;
}

int gyro_i2c_init()
{
	int busnum = 1;
	struct i2c_adapter *adap;
	struct i2c_client* client;
	trace("");
	adap = i2c_get_adapter(busnum);
	if (!adap) {
		error( "failed to get adapter i2c%d\n", busnum );
		return -ENODEV;
	}
	client = i2c_new_device(adap, &gyro_i2c_board_info);
	mutex_lock( &g_I2CLock );
	g_Client = client;
	mutex_unlock( &g_I2CLock );
	if( !client ) {
		error( "failed to register %s to i2c%d\n", gyro_i2c_board_info.type, busnum);
	}
	if( gyro_init_settings() ) {
		mutex_lock( &g_I2CLock );
		g_Client = NULL;
		mutex_unlock( &g_I2CLock );
		i2c_unregister_device( client );
		return -EIO;
	}

	return i2c_add_driver( &gyro_i2c_driver );
}

void gyro_i2c_exit()
{
	struct i2c_client* client;
	trace("");
	mutex_lock( &g_I2CLock );
	client = g_Client;
	g_Client = NULL;
	mutex_unlock( &g_I2CLock );
	if( client ) {
		i2c_unregister_device( client );
	}
	i2c_del_driver( &gyro_i2c_driver );
}

