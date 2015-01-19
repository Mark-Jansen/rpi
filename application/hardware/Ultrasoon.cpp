#include "Ultrasoon.h"
#include <iostream>
#include <ultrasoon/ultrasoon.h>
#include "BalancerDefines.h"
#include <generic/Logger.h>

Ultrasoon::Ultrasoon(int triggerPin, int echoPin,int Type)
	: mSensor( ULTRASOON_DEVICE, O_RDWR )
	, mTriggerPin( triggerPin )
	, mEchoPin( echoPin )
	, mType( Type)
{  
	//Set config IO setup
	ultrasoon_config cfg= {0};
	
	if(Type == 0)
	{
		cfg.pinNr_Trigger_1 = mTriggerPin;
		cfg.pinNr_echo_1 = mEchoPin;
	}
	else if(Type ==1)
	{
		cfg.pinNr_Trigger_2 = mTriggerPin;
		cfg.pinNr_echo_2 = mEchoPin;
	}
	else
		WARN("Unknow Type:" << Type);

	if( !mSensor.ioctl( ULTRASOON_SET_CONFIG, cfg ) ){
		WARN("Ultrasoon set config ioctl failed");
	}

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
	data.type = mType;

	mSensor.ioctl(ULTRASOON_GET_DISTANCE,data);
	
	return data.distance;
	
}

