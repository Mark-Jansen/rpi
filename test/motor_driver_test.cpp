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
#include <iostream>

#include "../modules/gpio/gpio.h"
#include "../modules/pwm/pwm.h"
#include "../modules/motor_driver/motor_driver.h"
using namespace std;
// =================================================================================
// ====   S T R U C T S                                                         ====
// =================================================================================
struct motor_driver_setting motor_A;   // with sw pwm
struct motor_driver_setting motor_B;	  // with hw pwm

// =================================================================================
// ====   F U N C T I O N S                                                     ====
// =================================================================================

void showMenu( void )
{
    cout << ("\n\nMENU");
	cout << ("\n===========================");
	cout << ("\n[1] Set SPEED motor A.");
	cout << ("\n[2] Set DIRECTION motor A");
	cout << ("\n[3] Set SPEED motor B.");
	cout << ("\n[4] Set DIRECTION motor B.");
	cout << ("\n[5] Set DIRECTION motor A & B.");
	cout << ("\n[6] Set SPEED motor A & B.");
	cout << ("\n[7] STOP");
	cout <<  ("\n\nKeuze : ");
}

int write(int fd,struct motor_driver_setting* config)
{	
	printf ("MotorApinL: %d \n", motor_A.direction_pinL);
	printf ("MotorApinR: %d \n", motor_A.direction_pinR);
	if( ioctl(fd, MOTOR_DRIVER_SET_CONFIG ,config ) == -1) { 
 		perror( "ioctl set" ); 
 		close( fd ); 
 		return -1; 
	}  
 	return 0; 
}

int setSpeed(int fd, struct motor_driver_setting* config)
{
	printf ("dutycycle_motor_A: %d \n", motor_A.pwm_duty_cycle);
	if( ioctl(fd, MOTOR_SETSPEED,config ) == -1) { 
 		perror( "set speed" ); 
 		close( fd ); 
 		return -1; 
	}  
 	return 0; 
}

void init_motor_driver_setting()
{
		//23,24,25,7
		motor_A.direction_in1_pinnr = 7;
		motor_A.direction_in2_pinnr = 23;
		motor_A.direction_pinL = 0;
		motor_A.direction_pinR = 0;
		motor_A.pwm_channel = 1;
		motor_A.pwm_pinnr = 17;
		motor_A.pwm_enable = 1;
		motor_A.pwm_frequency = 10000;
		motor_A.pwm_duty_cycle = 0;
		
		motor_B.direction_in1_pinnr = 24;
		motor_B.direction_in2_pinnr = 25;
		motor_B.direction_pinL = 0;
		motor_B.direction_pinR = 0;
		motor_B.pwm_channel = 0;
		motor_B.pwm_pinnr = 18;
		motor_B.pwm_enable = 1;
		motor_B.pwm_frequency = 10000;
		motor_B.pwm_duty_cycle = 0;	
		
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
	int dutycycle = 0;
	char direction;

	
	int fd = open("/dev/motor_driver", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}
	
	init_motor_driver_setting();
	write(fd,&motor_A);
	write(fd,&motor_B);
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
				cin >> dutycycle;
				cin.ignore();
				motor_A.pwm_duty_cycle = dutycycle;
				setSpeed(fd,&motor_A);
				break;	
			case '2':
				cout << "Set direction motor A (SW_PWM),[L] or [R]" << endl;
				cin >> direction;
				cin.ignore();
				if (!( direction != 'L' && direction != 'l'))
				{
					motor_A.direction_pinL = 1;
					motor_A.direction_pinR = 0;
					write(fd,&motor_A);
				}
				else if (!( direction != 'R' && direction != 'r'))
				{
					motor_A.direction_pinL = 0;
					motor_A.direction_pinR = 1;
					write(fd,&motor_A);
				}
				break;
			case '3':
				cout << "Set duty cycle Motor B,[0-100]" << endl;
				cin >> dutycycle;
				cin.ignore();
				motor_B.pwm_duty_cycle = dutycycle;
				setSpeed(fd,&motor_B);
				break;
			case '4':
				cout << "Set direction motor B (HW_PWM),[L] or [R]" << endl;
				cin >> direction;
				cin.ignore();
				if (!( direction != 'L' && direction != 'l'))
				{
					motor_B.direction_pinL = 1;
					motor_B.direction_pinR = 0;
					write(fd,&motor_B);
				}
				else if (!( direction != 'R' && direction != 'r'))
				{
					motor_B.direction_pinL = 0;
					motor_B.direction_pinR = 1;
					write(fd,&motor_B);
				}
				break;
			case '5':
				 // set direction motor A and B
				cout << "Set direction motor A (SW_PWM) & motor B (HW_PWM),[L] or [R]" << endl;
				cin >> direction;
				cin.ignore();
				if (!( direction != 'L' && direction != 'l'))
				{
					motor_A.direction_pinL = 1;
					motor_A.direction_pinR = 0;
					motor_B.direction_pinL = 1;
					motor_B.direction_pinR = 0;
					write(fd,&motor_A);
					write(fd,&motor_B);
				}
				else if (!( direction != 'R' && direction != 'r'))
				{
					motor_A.direction_pinL = 0;
					motor_A.direction_pinR = 1;
					motor_B.direction_pinL = 0;
					motor_B.direction_pinR = 1;
					write(fd,&motor_A);
					write(fd,&motor_B);
				}
				break;
			case '6':
				// set speed motor A and B
				cout << "Set duty cycle Motor A & B,[0-100]" << endl;			
				cin >> dutycycle;
				cin.ignore();
				motor_A.pwm_duty_cycle = dutycycle;
				motor_B.pwm_duty_cycle = dutycycle;
				setSpeed(fd,&motor_A);
				setSpeed(fd,&motor_B);
				break;
			
			
			case '7':
				close_menu = true;
				close(fd);
				break;	
		}		
	}	
}
