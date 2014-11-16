// =================================================================================
// ====                                                                         ====
// ==== File      : motor_driver.cpp                                            ====
// ====                                                                         ====
// ==== Function  : test motor_driver                                           ====
// ====                                                                         ====
// ==== Author    : Hung Tran     			                                    ====
// ====                                                                         ====
// ==== History   :  Version 1.00                                               ====
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

#include "../modules/gpio/gpio.h"
#include "../modules/pwm/pwm.h"

// =================================================================================
// ====   S T R U C T S                                                         ====
// =================================================================================
struct motor hw_pwm;
struct pwm_settings sw_pwm;
struct gpio_status ain1;
struct gpio_status ain2;
struct gpio_status bin1;
struct gpio_status bin2;

// =================================================================================
// ====   F U N C T I O N S                                                     ====
// =================================================================================

void showMenu( void )
{
    cout << ("\n\nMENU");
	cout << ("\n===========================");
	cout << ("\n[1] Set speed motor A.");
	cout << ("\n[2] Set direction motor A");
	cout << ("\n[3] Set speed motor B.");
	cout << ("\n[4] Set direction motor B.");
	cout << ("\n[5] Stop");
	cout <<  ("\n\nKeuze : ");
}

int write(int fd,struct pwm)
{

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
	int dutycycle_motor_A = 0;
	int dutycycle_motor_B = 0;
	char direction_A = "";
	char direction_B = "";
	
	while(!close_menu)
	{
		showMenu();
		cin.get(choice);
		cout << "Choice = " << choice << endl;
		cin.ignore();
		
		switch(choice)
		{
			case '1':
				cout << "Set duty cycle Motor A,[0-100]" << endl;
				cin >> dutycycle_motor_A;
				cin.ignore():
				//TODO IOCTL
				break;	
			case '2':
				cout << "Set direction motor A,[L] or [R]" << endl;
				cin >> direction_A;
				//TODO IOCTL direction motor A
				break;
			case '3':
				cout << "Set duty cycle Motor B,[0-100]" << endl;
				cin >> dutycycle_motor_B;
				cin.ignore():
				//TODO IOCTL
				break;
			case '4':
				cout << "Set direction motor B,[L] or [R]" << endl;
				cin >> direction_B;
				break;
			case '5':
				close_menu = true;
				break;
		
		
		}
		
	 

	}
	
}
