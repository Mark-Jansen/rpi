#include "Motor.h"

Motor::Motor( const Config& cfg )
	: mSensor( "/dev/motor", O_RDWR )
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
}

int Motor::getSpeed(void)
{
}
    
void Motor::setDirection(int direction)
{
}

int Motor::getDirection(void)
{
}
	
int Motor::readEncoder(void) //TODO
{
}

void Motor::turnOn(void)
{
}

void Motor::turnOff(void)
{
}
