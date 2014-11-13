#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "../modules/gpio/gpio.h"

int main()
{
	// set readyled without defines
	gpio_status status;
	status.pinNr = 24;
	status.value = 0;
	status.function = INPUT;
	
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
	

	if( ioctl(fd, GPIO_READ, &status ) == -1) 
	{
		perror( "ioctl GPIO_READ" );
		close( fd );
		return -1;
	}
	printf("Input pinNr \t= %i \n", status.pinNr);
	printf("Input value \t= %i \n", status.value);
	printf("Input function \t= %i \n", status.function);

	close( fd );
}
