#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "../modules/gpio/gpio.h"

int main()
{
	// input pin
	gpio_status button;
	button.pinNr = 24;
	button.value = INPUT;
	button.function = 1;
	
	// output pin	
	gpio_status led_orange;
	led_orange.pinNr = 23;
	led_orange.value = OUTPUT;
	led_orange.function = 0;
	
	int value;
	
	int fd = open("/dev/gpio", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}
	
	// init input pin
	if( ioctl(fd, GPIO_SET_CONFIG, &button ) == -1) 
	{
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	// init output pin
	if( ioctl(fd, GPIO_SET_CONFIG, &led_orange ) == -1) 
	{
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	while (1)
	{
		// read input pin
		if( ioctl(fd, GPIO_READ, &button ) == -1) 
		{
			perror( "ioctl set" );
			close( fd );
			return -1;
		}
		
		// check value of input pin and set value of output pin
		value = button.value;
		led_orange.value = value;
			
		// write value to output pin
		if( ioctl(fd, GPIO_WRITE, &led_orange ) == -1) 
		{
			perror( "ioctl set" );
			close( fd );
			return -1;
		}
		// print input
		printf("button pinNr \t \t= %i \n", 	button.pinNr);
		printf("button value  \t \t= %i \n", 	button.value);
		printf("button function \t= %i \n", 	button.function);

		// print output
		printf("led_orange pinNr \t= %i \n", 	led_orange.pinNr);
		printf("led_orange value \t= %i \n", 	led_orange.value);
		printf("led_orange function \t= %i \n \n", led_orange.function);				
	}
	close( fd );
}
