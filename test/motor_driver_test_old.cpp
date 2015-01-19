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
#include "../modules/encoder/encoder.h"
#include "../modules/motor_driver/motor_driver.h"
using namespace std;
// =================================================================================
// ====   S T R U C T S                                                         ====
// =================================================================================
struct motor_driver_setting motor_A;   // with sw pwm
struct motor_driver_setting motor_B;	  // with hw pwm
struct encoder_data encoder_A;
struct encoder_data encoder_B;

// =================================================================================
// ====   F U N C T I O N S                                                     ====
// =================================================================================

void showMenu( void )
{
	cout << ("\n================================");
	cout << ("\n\nMENU");
	cout << ("\n================================");
	cout << ("\n[1] Set SPEED motor A.");
	cout << ("\n[2] Set DIRECTION motor A");
	cout << ("\n[3] Set SPEED motor B.");
	cout << ("\n[4] Set DIRECTION motor B.");
	cout << ("\n================================");
	cout << ("\n[5] Set DIRECTION motor A & B.");
	cout << ("\n[6] Set SPEED motor A & B.");
	cout << ("\n================================");
	cout << ("\n[7] Get Motor SPEED A & B");
	cout << ("\n[8] Get Motor DIRECTION A & B.");
	cout << ("\n================================");
	cout << ("\n[9] STOP");
	cout <<  ("\n\nKeuze : ");
}

int write(int fd,struct motor_driver_setting* config)
{	
	printf ("MotorApinL: %d \n", motor_A.direction_pinL);
	printf ("MotorApinR: %d \n", motor_A.direction_pinR);
	if( ioctl(fd, MOTOR_DRIVER_SET_SETTINGS ,config ) == -1) { 
 		perror( "ioctl set" ); 
 		close( fd ); 
 		return -1; 
	}  
 	return 0; 
}

int write_encoder(int fd, struct encoder_data* setting)
{
	if( ioctl(fd, ENCODER_SET_CONFIG ,setting ) == -1) { 
 		perror( "ioctl set" ); 
 		close( fd ); 
 		return -1; 
	}  
 	return 0; 
}

 int setSpeed(int fd, struct motor_driver_setting* config)
 {
	 if( ioctl(fd, MOTOR_DRIVER_SET_SPEED,config ) == -1) { 
 		 perror( "set speed" ); 
		 close( fd ); 
 		 return -1; 
	 }  
 	 return 0; 
 }

int getSpeed( int fd, motor_driver_setting *mded  )
{
	if( ioctl(fd, MOTOR_DRIVER_GET_SPEED , mded ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	return 0;
}

int setDirection(int fd, motor_driver_setting* config)
{
if( ioctl(fd, MOTOR_DRIVER_SET_DIRECTION , config ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	return 0;
}

int getDirection(int fd, motor_driver_setting* config)
{
if( ioctl(fd, MOTOR_DRIVER_GET_DIRECTION , config ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	//printf ("direction A: %d \n", motor_A.direction);
	//printf ("direction B: %d \n", motor_B.direction);
	return 0;
}

void init_motor_driver_setting()
{
	
		motor_A.direction_in1_pinnr = 27;
		motor_A.direction_in2_pinnr = 22;
		motor_A.direction_pinL = 0;
		motor_A.direction_pinR = 0;
		motor_A.pwm_channel = 1;
		motor_A.pwm_pinnr = 23;
		motor_A.pwm_enable = 1;
		motor_A.pwm_frequency = 25000;
		motor_A.pwm_duty_cycle = 0;
		motor_A.speed = 0;
		motor_A.direction = 0;      // 0 is left , 1 is right
		
		motor_B.direction_in1_pinnr = 4;
		motor_B.direction_in2_pinnr = 17;
		motor_B.direction_pinL = 0;
		motor_B.direction_pinR = 0;
		motor_B.pwm_channel = 0;
		motor_B.pwm_pinnr = 18;
		motor_B.pwm_enable = 1;
		motor_B.pwm_frequency = 25000;
		motor_B.pwm_duty_cycle = 0;
		motor_B.speed = 0;
		motor_B.direction = 0;      // 0 is left , 1 is right		
}


void init_encoder_setting()
{
	encoder_A.encoder1_pinnr = 10;
	encoder_A.encoder2_pinnr = 9;
	encoder_B.encoder1_pinnr = 8;
	encoder_B.encoder2_pinnr = 7;	
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
	//bool stop_reading_speed = false;
	char choice = 0;
	int dutycycle = 0;
	char direction;

	
	int fd_motor_A = open("/dev/motor_driver", O_RDWR);
	int fd_motor_B = open("/dev/motor_driver", O_RDWR);
	int fd_encoder_A = open("/dev/encoder", O_RDWR); 
	int fd_encoder_B = open("/dev/encoder", O_RDWR); 
	if (fd_motor_A == -1) {
		perror( "open" );
		return 1;
	}
	
	if (fd_motor_B == -1) {
		perror( "open" );
		return 1;
	}
	if (fd_encoder_A == -1) {
		perror( "open" );
		return 1;
	}
	
	if (fd_encoder_B == -1) {
		perror( "open" );
		return 1;
	}
	
	init_motor_driver_setting();
	init_encoder_setting();
	write(fd_motor_A,&motor_A);
	write(fd_motor_B,&motor_B);
	write_encoder(fd_encoder_A,&encoder_A);
	write_encoder(fd_encoder_B,&encoder_B);
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
				setSpeed(fd_motor_A,&motor_A);
				break;	
			case '2':
				cout << "Set direction motor A (SW_PWM),[L] or [R]" << endl;
				cin >> direction;
				cin.ignore();
				if (!( direction != 'L' && direction != 'l'))
				{
					motor_A.direction_pinL = 1;
					motor_A.direction_pinR = 0;
					setDirection(fd_motor_A,&motor_A);			
				}
				else if (!( direction != 'R' && direction != 'r'))
				{
					motor_A.direction_pinL = 0;
					motor_A.direction_pinR = 1;
					setDirection(fd_motor_A,&motor_A);	
				}
				break;
			case '3':
				cout << "Set duty cycle Motor B,[0-100]" << endl;
				cin >> dutycycle;
				cin.ignore();
				motor_B.pwm_duty_cycle = dutycycle;
				setSpeed(fd_motor_B,&motor_B);
				break;
			case '4':
				cout << "Set direction motor B (HW_PWM),[L] or [R]" << endl;
				cin >> direction;
				cin.ignore();
				if (!( direction != 'L' && direction != 'l'))
				{
					motor_B.direction_pinL = 1;
					motor_B.direction_pinR = 0;
					setDirection(fd_motor_B,&motor_B);
				}
				else if (!( direction != 'R' && direction != 'r'))
				{
					motor_B.direction_pinL = 0;
					motor_B.direction_pinR = 1;
					setDirection(fd_motor_B,&motor_B);
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
					setDirection(fd_motor_A,&motor_A);
					setDirection(fd_motor_B,&motor_B);
				}
				else if (!( direction != 'R' && direction != 'r'))
				{
					motor_A.direction_pinL = 0;
					motor_A.direction_pinR = 1;
					motor_B.direction_pinL = 0;
					motor_B.direction_pinR = 1;
					setDirection(fd_motor_A,&motor_A);
					setDirection(fd_motor_B,&motor_B);
				}
				break;
			case '6': // set speed motor A and B	
				cout << "Set duty cycle Motor A & B,[0-100]" << endl;			
				cin >> dutycycle;
				cin.ignore();
				motor_A.pwm_duty_cycle = dutycycle;
				motor_B.pwm_duty_cycle = dutycycle;
				setSpeed(fd_motor_A,&motor_A);
				setSpeed(fd_motor_B,&motor_B);
				break;
			case '7': // get speed motor A & B			
				getSpeed(fd_motor_A,&motor_A);
				cout << "Speed motor A = " << motor_A.speed << endl;
				getSpeed(fd_motor_B,&motor_B);
				cout << "Speed motor B = " << motor_B.speed << endl;
				break;
			case '8':
				getDirection(fd_motor_A,&motor_A);
				getDirection(fd_motor_B,&motor_B);
				cout << "Direction motor A = " << motor_A.direction << endl;
				cout << "Direction motor B = " << motor_B.direction << endl;
				break;
			
			case '9':
				close_menu = true;
				close(fd_encoder_A);
				close(fd_encoder_B);
				close(fd_motor_A);
				close(fd_motor_B);
				break;	
		}		
	}	
}
