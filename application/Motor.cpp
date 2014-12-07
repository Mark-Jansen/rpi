#include "Motor.h"

Motor::Motor(void)
	: mSensor( "/dev/motor", O_RDWR )
{  
}

Motor::~Motor() 
{
}

void Motor::start(void)
{
}

void Motor::stop(void)
{
}

