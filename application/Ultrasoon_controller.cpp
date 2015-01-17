#include "Ultrasoon_controller.h"
#include <hardware/Ultrasoon.h>
#include <iostream>

#include "BalancerDefines.h"
#include <generic/Logger.h>

Ultrasoon_controller::Ultrasoon_controller()
{
	mFront = new Ultrasoon( ULTRASOON_F_TRIGGER, ULTRASOON_F_ECHO );
	mBack = new Ultrasoon( ULTRASOON_B_TRIGGER, ULTRASOON_B_ECHO );
}

Ultrasoon_controller::~Ultrasoon_controller() 
{
	delete mFront;
	delete mBack;
}

int Ultrasoon_controller::delayMS() const
{
	return 500;	// Minimum timeout
}

void Ultrasoon_controller::onBeforeRun()
{
	//std::cerr << "Initialize ultrasoon" << std::endl;
	if( !mFront->isInitialized() ) {
		ERR("Could not initialize front Ultrasoon");
		stop();
	}
	if( !mBack->isInitialized() ) {
		ERR("Could not initialize back Ultrasoon");
		stop();
	}
}

void Ultrasoon_controller::onRun()
{
	//std::cerr << "read ultrasoon" << std::endl;
	//std::cerr << "process data" << std::endl;
	//std::cerr << "store data" << std::endl;
}
