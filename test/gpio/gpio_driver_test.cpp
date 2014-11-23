// =================================================================================
// ====                                                                         ====
// ==== File      : gpio_driver_test.cpp                                        ====
// ====                                                                         ====
// ==== Function  : test gpio_driver                                            ====
// ====                                                                         ====
// ==== Author    : Stefan van Nunen    			                            ====
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


// =================================================================================
// ====   S T R U C T S                                                         ====
// =================================================================================
struct gpio_status gpio_test;
using namespace std;

// =================================================================================
// ====   F U N C T I O N S                                                     ====
// =================================================================================

void showMenu( void )
{
	printf("\n\nMENU");
	printf("\n===========================");
	printf("\n[1] Set config function");
	printf("\n[2] write value");
	printf("\n[3] read value");
	printf("\n[4] set pull up/down");
	printf("\n[5] Stop");
	printf("\n\nKeuze : ");
}

void showFunctionMenu( void )
{
	printf("\n\nFunction MENU");
	printf("\n=============");
	printf("\n[1] INPUT");
	printf("\n[2] OUTPUT");
	printf("\n[3] ALT0");
	printf("\n[4] ALT1");
	printf("\n[5] ALT2");
	printf("\n[6] ALT3");
	printf("\n[7] ALT4");
	printf("\n[8] ALT5");
	printf("\n[9] Stop");
	printf("\n\nKeuze : ");
}

void showPullMenu( void )
{
	printf("\n\nPull up/down MENU");
	printf("\n=============");
	printf("\n[1] pull up");
	printf("\n[2] pull down");
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
	int pinnr = 0;
	int function = 0;
	int value = 0;
	
	int fd = open("/dev/gpio", O_RDWR);
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
				cout << "Pinnumber to config" << endl;
				cin >> pinnr;
				cin.ignore();
				gpio_test.pinNr = pinnr;
				
				showFunctionMenu();
				cin.get(choice);
				cin.ignore();
				switch(choice)
				{
					case '1':
					gpio_test.function = INPUT;
					break;
					case '2':
					gpio_test.function = OUTPUT;
					break;
					case '3':
					gpio_test.function = ALT0;
					break;
					case '4':
					gpio_test.function = ALT1;
					break;
					case '5':
					gpio_test.function = ALT2;
					break;	
					case '6':
					gpio_test.function = ALT3;
					break;
					case '7':
					gpio_test.function = ALT4;
					break;
					case '8':
					gpio_test.function = ALT5;	
					break;
				}				
				cout << "youre pinnumber = " << gpio_test.pinNr << endl;
				cout << "youre function = " << gpio_test.function << endl;
				if( ioctl(fd, GPIO_SET_CONFIG, &gpio_test) == -1) 
				{
					perror( "ioctl GPIO_SET_CONFIG:  failed" );
					close( fd );
					return -1;
				}
				break;	
			case '2':
				cout << "pinnumber to change value" << endl;
				cin >> pinnr;
				cin.ignore();
				cout << "change value to:" << endl;
				cin >> value;
				cin.ignore();
				gpio_test.pinNr = pinnr;
				gpio_test.value = value;
				gpio_test.function = OUTPUT;
				if( ioctl(fd, GPIO_SET_CONFIG, &gpio_test) == -1) 
				{
					perror( "ioctl GPIO_SET_CONFIG:  failed" );
					close( fd );
					return -1;
				}
				if( ioctl(fd, GPIO_WRITE, &gpio_test) == -1) 
				{
					perror( "ioctl GPIO_WRITE:  failed" );
					close( fd );
					return -1;
				}
				break;
			case '3':
				cout << "pinnumber to read value" << endl;
				cin >> pinnr;
				cin.ignore();
				gpio_test.function = INPUT;
				gpio_test.pinNr = pinnr;
				if( ioctl(fd, GPIO_SET_CONFIG, &gpio_test) == -1) 
				{
					perror( "ioctl GPIO_SET_CONFIG:  failed" );
					close( fd );
					return -1;
				}
				if( ioctl(fd, GPIO_READ, &gpio_test) == -1) 
				{
					perror( "ioctl GPIO_READ:  failed" );
					close( fd );
					return -1;
				}
				cout << "value of gpiopin :" << gpio_test.pinNr << " = "<<  gpio_test.value << endl;
				break;	
			case '4':
				cout << "Pinnumber to set pull_up_down" << endl;
				cin >> pinnr;
				cin.ignore();
				gpio_test.pinNr = pinnr;
				gpio_test.function = INPUT;
				
				showPullMenu();
				cin.get(choice);
				cin.ignore();
				switch(choice)
				{
					case '1':
					gpio_test.pull_up_down = PULL_UP_ENABLE;
					break;
					case '2':
					gpio_test.pull_up_down = PULL_DOWN_ENABLE;
					break;
				}				
				cout << "youre pinnumber = \t" << gpio_test.pinNr << endl;
				cout << "youre function = \t" << gpio_test.function << endl;
				cout << "youre pull up/down = \t" << gpio_test.pull_up_down << endl;
				if( ioctl(fd, GPIO_SET_CONFIG, &gpio_test) == -1) 
				{
					perror( "ioctl GPIO_SET_CONFIG:  failed" );
					close( fd );
					return -1;
				}
				break;	
			case '5':
				close_menu = true;
				close ( fd );
				break;
		
		}
		
	 

	}
	
}
