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
	std::cerr << "Initialize ultrasoon" << std::endl;
	
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
	int distaneValue = 0;
	std::cerr << "Distance was: " << distaneValue << "cm"<< std::endl;
	std::cerr << "reading Front ultrasoon sensor" << std::endl;
	distaneValue = mFront->getDistance();
	std::cerr << "Distance: " << distaneValue << "cm"<< std::endl;
	
	std::cerr << "read Back ultrasoon sensor" << std::endl;
	distaneValue = mBack->getDistance();
	std::cerr << "Distance: " << distaneValue << "cm"<< std::endl;
	
	//std::cerr << "process data" << std::endl;
	//std::cerr << "store data:" << std::endl;
	
	
	
}












