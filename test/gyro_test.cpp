#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <gyro/gyro.h>
#include <string>
#include "../application/File.h"


bool read( File& gyro, gyro_data& data )
{
	if( !gyro.ioctl( GYRO_GET_DATA, data ) ) {
		perror( "ioctl get" );
		return false;
	}
	return true;
}

int main()
{
	gyro_data data;
	File gyro("/dev/gyro", O_RDWR);
	if( !gyro.isOpen() ) {
		perror( "open" );
		return 1;
	}
	for( int n = 0; n < 100; ++n ) {
		if( read( gyro, data ) ) {
			printf( "Accel: %d, %d, %d\nTemp: %f\nGyro: %d, %d, %d\n\n",
				data.accel_x, data.accel_y, data.accel_z, data.raw_temperature / 340.f + 36.53f,
				data.gyro_x, data.gyro_y, data.gyro_z );
		}
	}
	return 0;
}
