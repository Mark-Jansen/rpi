#include "Battery.h"

Battery::Battery( Led& warningLed )
	: mSensor( "/dev/battery", O_RDWR )
	, mWarningLed( warningLed )
{  
}

Battery::~Battery() 
{
}

int Battery::delayMS() const
{
	return 5 * 1000;
}

void Battery::onBeforeRun()
{
}

void Battery::onRun()
{
}
