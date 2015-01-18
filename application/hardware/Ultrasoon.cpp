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
//Set config IO setup
	ultrasoon_config cfg= {0};
	cfg.pinNr_Trigger = mTriggerPin;
	cfg.pinNr_echo_1 = mEchoPin;
	
	mSensor.ioctl( ULTRASOON_SET_CONFIG, cfg );

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
	
	
	//if( !mSensor.ioctl( ULTRASOON_SET_CONFIG,cfg ) )
	//	return false;
	

	return mSensor.isOpen();

}

int Ultrasoon::getDistance(void)
{
	ultrasoon_data data = {0};
	
	mSensor.ioctl(ULTRASOON_GET_DISTANCE,data);
	return data.distance;
	
	//std::cerr << "TODO: " << __PRETTY_FUNCTION__ << std::endl;
>>>>>>> master
}

