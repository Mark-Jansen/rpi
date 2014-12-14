#include "Ultrasoon.h"

Ultrasoon::Ultrasoon(int triggerPin, int echoPin, int type)
	: mSensor( "/dev/ultrasoon", O_RDWR )
{  
}

Ultrasoon::~Ultrasoon() 
{
}

int Ultrasoon::getDistance(void)
{
}

