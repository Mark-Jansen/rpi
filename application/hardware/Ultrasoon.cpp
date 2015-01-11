#include "Ultrasoon.h"

#include <iostream>

#include <ultrasoon/ultrasoon.h>
#include "BalancerDefines.h"

Ultrasoon::Ultrasoon(int triggerPin, int echoPin, int type)
	: mSensor( ULTRASOON_DEVICE, O_RDWR )
	, mTriggerPin( triggerPin )
	, mEchoPin( echoPin )
	, mType( type )
{  
}

Ultrasoon::~Ultrasoon() 
{
}

bool Ultrasoon::isInitialized() const
{
	return mSensor.isOpen();
}

int Ultrasoon::getDistance(void)
{
	//std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}

