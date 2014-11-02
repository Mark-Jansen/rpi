#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "gpio.h"

int main()
{
	// input pin
	gpio_status input;
	input.pinNr = 24;
	input.value = INPUT;
	input.function = 1;
	
	// output pin	
	gpio_status output;
	output.pinNr = 23;
	output.value = OUTPUT;
	output.function = 0;
	
	int value;
	
	int fd = open("/dev/gpio", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}
	
	// init input pin
	if( ioctl(fd, GPIO_SET_CONFIG, &input ) == -1) 
	{
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	// init output pin
	if( ioctl(fd, GPIO_SET_CONFIG, &output ) == -1) 
	{
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	while (1)
	{
		// read input pin
		if( ioctl(fd, GPIO_READ, &input ) == -1) 
		{
			perror( "ioctl set" );
			close( fd );
			return -1;
		}
		
		// check value of input pin and set value of output pin
		value = input.value;
		output.value = value;
			
		// write value to output pin
		if( ioctl(fd, GPIO_WRITE, &output ) == -1) 
		{
			perror( "ioctl set" );
			close( fd );
			return -1;
		}
		// print input
		printf("Input pinNr \t \t= %i \n", 	input.pinNr);
		printf("Input value  \t \t= %i \n", 	input.value);
		printf("Input function \t \t= %i \n", 	input.function);

		// print output
		printf("Outnput pinNr \t \t= %i \n", 	output.pinNr);
		printf("Output value \t \t= %i \n", 	output.value);
		printf("Output function \t= %i \n \n", output.function);				
	}
	close( fd );
}
