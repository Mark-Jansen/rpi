#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <gpio/gpio.h>


int read( int fd, struct example_status* status )
{
	if( ioctl(fd, EXAMPLE_GET_STATUS, status ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	printf( "Status: %d\n", status->status );
	return 0;
}

int write( int fd, struct example_status* status )
{
	if( ioctl(fd, EXAMPLE_SET_STATUS, status ) == -1) {
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	return 0;
}

int main()
{
/*	// set readyled pin with defines
	gpio_status status;
	status.channel = READY_LED;
	status.value = 1;
	status.function = OUTPUT;
*/	
	// set readyled without defines
	gpio_status status;
	status.channel = 18;
	status.value = 1;
	status.function = 0;
	
	int fd = open("/dev/gpio", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}

	if( ioctl(fd, GPIO_SET_CONFIG, status ) == -1) 
	{
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	if( ioctl(fd, GPIO_WRITE, status ) == -1) 
	{
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	close( fd );
}