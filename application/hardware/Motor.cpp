#include "Motor.h"
#include <motor_driver/motor_driver.h> //TODO
#include <iostream>
#include <pwm/pwm.h>
#include "BalancerDefines.h"
#include <generic/Logger.h>

//TODO
//this application driver uses motor.h (from linux kernel module)
//commented code can be uncommented 

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
    motor_driver_setting md;
	mSensor.ioctl(MOTOR_DRIVER_GET_SETTINGS, md);
	md.pwm_duty_cycle = speed;
	mSpeed = speed; //save speed in object motor
	if( !mSensor.ioctl( MOTOR_DRIVER_SET_SPEED, md ) ) {   
		ERR("Writing setSpeed to motor on channel: " << md.pwm_channel);
	}
}

int Motor::getSpeed(void)
{
	return mSpeed; //TODO
}
    
void Motor::setDirection(Dir d)
{
    motor_driver_setting md;
	mSensor.ioctl(MOTOR_DRIVER_GET_SETTINGS, md);
	switch( d ) {
	case Dir::Forward:
	   md.direction_pinL = 1; //check this
	   md.direction_pinR = 0;
		break;
	case Dir::Backward:
	   md.direction_pinL = 0;
	   md.direction_pinR = 1;	
		break;
	case Dir::None:
	   //md.direction_pinL = 
	   //md.direction_pinR =
		break;
	}
	if( !mSensor.ioctl( MOTOR_DRIVER_SET_SETTINGS, md ) ) {
		ERR("Writing setSetdirection to motor on channel: " << md.pwm_channel);
	}
	
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
    motor_driver_setting md;
	mSensor.ioctl(MOTOR_DRIVER_GET_SETTINGS, md);
	md.pwm_enable = true;
	mEnabled = true;
	
	if( !mSensor.ioctl( MOTOR_DRIVER_SET_SETTINGS, md ) ) {
		ERR("Writing turnOff to motor on channel: " << md.pwm_channel);
	}
}

void Motor::turnOff(void)
{
    motor_driver_setting md;
	mSensor.ioctl(MOTOR_DRIVER_GET_SETTINGS, md);
	md.pwm_enable = false;
	mEnabled = false;
	
	if( !mSensor.ioctl( MOTOR_DRIVER_SET_SETTINGS, md ) ) {
		ERR("Writing turnOff to motor on channel: " << md.pwm_channel);
	}
}
