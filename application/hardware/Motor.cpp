#include "Motor.h"
//#include <motor/motor.h> //TODO

//TODO
//motordriver has to be pushed to master
//this application driver uses motor.h (from linux kernel module)
//commented code can be uncommented 



#include <iostream>

#include <pwm/pwm.h>
#include "BalancerDefines.h"

#define FORWARD			1
#define BACKWARD 		2
#define NONE	 		0


Motor::Motor( const Config& cfg )
	: mSensor( PWM_DEVICE, O_RDWR )
	, mConfig( cfg )
	, mSpeed( 0 )
	, mEnabled( 0 )
{  
}

Motor::~Motor() 
{
}

void Motor::setSpeed(int speed)
{
    /*
    motor_driver_data md = {0};
	md.pwm = speed;
	mSpeed = speed; //save speed in object motor
	if( !mSensor.ioctl( ioctl, md ) ) {   //TODO change ioctl to correct ioctl #define
		ERR("Writing setSpeed to motor on pwm pin" << mConfig.PwmPin);
	}
	*/
}

int Motor::getSpeed(void)
{
	return mSpeed;
}
    
void Motor::setDirection(Dir d)
{
    /*
	switch( d ) {
	case Dir::Forward:
	   //md.direction_pin1 = 
	   //md.direction_pin2 =
		break;
	case Dir::Backward:
	   //md.direction_pin1 = 
	   //md.direction_pin2 =
		break;
	case Dir::None:
	   //md.direction_pin1 = 
	   //md.direction_pin2 =
		break;
	}
	if( !mSensor.ioctl( ioctl, md ) ) { //TODO change ioctl to correct ioctl #define
		ERR("Writing setSetdirection to motor on pwm pin" << mConfig.PwmPin);
	}
	*/
}

Dir Motor::getDirection(void)
{
	
	
	//if      (direction1 == ...) && (direction2 == ...) return FORWARD;
	//else if (direction1 == ...) && (direction2 == ...) return BACKWARD;
	//else 											   return NONE;
}
	
int Motor::readEncoder(void) //TODO
{
	std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}

void Motor::turnOn(void)
{
    /*
	motor_driver_data md = {0};
	mEnabled = true;
	//todo turn motor off 
	*/
}

void Motor::turnOff(void)
{
    /*
	motor_driver_data md = {0};
	mEnabled = false;
	//todo turn motor off 
	*/
}
