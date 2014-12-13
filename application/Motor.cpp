#include "Motor.h"

Motor::Motor(void)
	: mSensor( "/dev/motor", O_RDWR )
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

void Motor::TurnOn(void)
{
}

void Motor::TurnOff(void)
{
}
