#include "Ultrasoon.h"

#include <iostream>

#include <ultrasoon/ultrasoon.h>
#include "BalancerDefines.h"

Ultrasoon::Ultrasoon(int triggerPin, int echoPin)
	: mSensor( ULTRASOON_DEVICE, O_RDWR )
	, mTriggerPin( triggerPin )
	, mEchoPin( echoPin )
{  
//Set config IO setup
	ultrasoon_config cfg= {0};
	cfg.pinNr_Trigger = mTriggerPin;
	cfg.pinNr_echo_1 = mEchoPin;
	
	mSensor.ioctl( ULTRASOON_SET_CONFIG, cfg );

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
	ultrasoon_data data = {0};
	
	mSensor.ioctl(ULTRASOON_GET_DISTANCE,data);
	return data.distance;
	
	//std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
}

