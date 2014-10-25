#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <adc/adc.h>


int read( int fd, struct adc_data* data )
{
	if( ioctl(fd, ADC_GET_DATA, data ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	printf( "Value: %d\n", data->value );
	return 0;
}

int main()
{
	adc_data data;
	int fd = open("/dev/adc", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}
	data.channel = 0;
	if( read( fd, &data ) ) {
		return 2;
	}
	data.channel = 1;
	if( read( fd, &data ) ) {
		return 3;
	}
	close( fd );
}
