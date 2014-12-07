#include "Ultrasoon.h"

Ultrasoon::Ultrasoon(int triggerPin, int echoPin)
	: mSensor( "/dev/ultrasoon", O_RDWR )
{  
}

Ultrasoon::~Ultrasoon() 
{
}

int Ultrasoon::delayMS() const
{
	return 100;
}

void Ultrasoon::calibrate()
{

}

void Ultrasoon::onBeforeRun()
{

}

void Ultrasoon::onRun()
{
}


