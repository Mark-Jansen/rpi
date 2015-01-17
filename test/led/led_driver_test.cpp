// =================================================================================
// ====                                                                         ====
// ==== File      : led_driver.cpp                                              ====
// ====                                                                         ====
// ==== Function  : test led_driver                                             ====
// ====                                                                         ====
// ==== Author    : Stefan van Nunen		                                    ====
// ====                                                                         ====
// ==== History   : Version 1.00                                                ====
// ====             								                            ====
// ====                     							                        ====
// ====                                                                         ====
// =================================================================================

// =================================================================================
// ====   I N C L U D E S                                                       ====
// =================================================================================
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include "../../modules/gpio/gpio.h"
#include "../../modules/led/led.h"


// =================================================================================
// ====   S T R U C T S                                                         ====
// =================================================================================
struct led_status off_led;
struct led_status on_led;
struct led_status blink_led;
using namespace std;

// =================================================================================
// ====   F U N C T I O N S                                                     ====
// =================================================================================

void showMenu( void )
{
	printf("\n\nMENU");
	printf("\n===========================");
	printf("\n[1] Set led on.");
	printf("\n[2] Set led off");
	printf("\n[3] Set led blink");
	printf("\n[5] Stop");
	printf("\n\nKeuze : ");
}


// =================================================================================
// ====   MAIN   F U N C T I O N                                                ====
// =================================================================================
// =================================================================================
// int main()
// Pre : -
// Post: Main program
// =================================================================================
int main()
{
	bool close_menu = false;
	char choice = 0;
	int pinnrON = 0;
	int pinnrOFF = 0;
	int pinnrBlink = 0;

	// default value
	on_led.pinNr = 23;
	on_led.value = ON;
	// default value
	off_led.pinNr = 23;
	off_led.value = OFF;
	
	blink_led.pinNr = 23;
	blink_led.value = OFF;
	
	int fd = open("/dev/led", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}
	
	while(!close_menu)
	{
		showMenu();
		cin.get(choice);
		cout << "Choice = " << choice << endl;
		cin.ignore();
		switch(choice)
		{
			case '1':
				cout << "Pinnumber to set on: " << endl;
				cin >> pinnrON;
				cin.ignore();
				on_led.pinNr = pinnrON;
				if( ioctl(fd, LED_SET_CONFIG, &on_led) == -1) 
				{
					perror( "ioctl LED_SET_CONFIG:  failed" );
					close( fd );
					return -1;
				}
				cout << "youre pinnumber = " << on_led.pinNr << endl;
				if( ioctl(fd, LED_ON, &on_led) == -1) 
				{
					perror( "ioctl LED_ON:  failed" );
					close( fd );
					return -1;
				}
				cout << "pinnumber should be on = " << on_led.pinNr << endl;
				break;	
			case '2':
				cout << "pinnumber to set off: " << endl;
				cin >> pinnrOFF;
				cin.ignore();
				off_led.pinNr = pinnrOFF;
				if( ioctl(fd, LED_SET_CONFIG, &off_led) == -1) 
				{
					perror( "ioctl LED_SET_CONFIG:  failed" );
					close( fd );
					return -1;
				}
				if( ioctl(fd, LED_OFF, &off_led) == -1) 
				{
					perror( "ioctl LED_ON:  failed" );
					close( fd );
					return -1;
				}
				break;
			case '3':
				cout << "pinnumber to blink: " << endl;
				cin >> pinnrBlink;
				cin.ignore();
				blink_led.pinNr = pinnrBlink;
				blink_led.blinkTimer = 500000.;
				if( ioctl(fd, LED_SET_CONFIG, &blink_led) == -1) 
				{
					perror( "ioctl LED_SET_CONFIG:  failed" );
					close( fd );
					return -1;
				}
				if( ioctl(fd, LED_BLINK, &blink_led) == -1) 
				{
					perror( "ioctl LED_BLINK:  failed" );
					close( fd );
					return -1;
				}
				break;					
			case '5':
				close_menu = true;
				break;
		
		}
		
	 

	}
	
}
