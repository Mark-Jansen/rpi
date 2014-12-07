#include "Battery.h"

Battery::Battery(void)
	: mSensor( "/dev/battery", O_RDWR )
{  
}

Battery::~Battery() 
{
}

int Battery::delayMS() const
{
	return 100;
}

void Battery::onBeforeRun()
{
}

void Battery::onRun()
{
}
