#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "../modules/gpio/gpio.h"
#include "../modules/led/led.h"

int main()
{
/**********************************************************************/	
	// set readyled without defines
	led_status led_red;
	led_red.pinNr = 23;
	led_red.value = ON;
	led_red.blinkTimer = SEC;

	int fd = open("/dev/led", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return -1;
	}

	if( ioctl(fd, LED_SET_CONFIG, &led_red) == -1) 
	{
		perror( "ioctl LED_SET_CONFIG: failed" );
		close( fd );
		return -1;
	}

	
	close( fd );
}
