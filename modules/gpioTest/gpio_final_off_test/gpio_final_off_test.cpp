#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "gpio.h"

int main()
{
	// set readyled without defines
	gpio_status status;
	status.pinNr = 18;
	status.value = 0;
	status.function = OUTPUT;
	
	int fd = open("/dev/gpio", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}

	if( ioctl(fd, GPIO_SET_CONFIG, &status ) == -1) 
	{
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	if( ioctl(fd, GPIO_WRITE, &status ) == -1) 
	{
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	close( fd );
}
