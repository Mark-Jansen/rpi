#include "Ultrasoon.h"

Ultrasoon::Ultrasoon(int triggerPin, int echoPin)
	: mSensor( "/dev/ultrasoon", O_RDWR )
{  
}

Ultrasoon::~Ultrasoon() 
{
}

Ultrasoon::getDistance(void)
{
}

