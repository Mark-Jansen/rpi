#include "Motor.h"

#include <iostream>

#include "BalancerDefines.h"

Motor::Motor( const Config& cfg )
	: mSensor( MOTOR_DEVICE, O_RDWR )
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
	std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}

int Motor::getSpeed(void)
{
	std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}
    
void Motor::setDirection(int direction)
{
	std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}

int Motor::getDirection(void)
{
	std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}
	
int Motor::readEncoder(void) //TODO
{
	std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}

void Motor::turnOn(void)
{
	std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}

void Motor::turnOff(void)
{
	std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}
