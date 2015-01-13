#include "Ultrasoon.h"

#include <iostream>

#include <ultrasoon/ultrasoon.h>
#include "BalancerDefines.h"

<<<<<<< HEAD
Ultrasoon::Ultrasoon(int triggerPin, int echoPin, int type)
	: mSensor( ULTRASOON_DEVICE, O_RDWR )
	, mTriggerPin( triggerPin )
	, mEchoPin( echoPin )
	, mType( type )
=======
Ultrasoon::Ultrasoon(int triggerPin, int echoPin)
	: mSensor( ULTRASOON_DEVICE, O_RDWR )
	, mTriggerPin( triggerPin )
	, mEchoPin( echoPin )
>>>>>>> master
{  
}

Ultrasoon::~Ultrasoon() 
{
}

<<<<<<< HEAD
int Ultrasoon::getDistance(void)
{
	std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
=======
bool Ultrasoon::isInitialized() const
{
	return mSensor.isOpen();
}

int Ultrasoon::getDistance(void)
{
	//std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
>>>>>>> master
}

