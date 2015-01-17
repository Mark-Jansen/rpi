#include "Ultrasoon_controller.h"
#include <hardware/Ultrasoon.h>
#include <iostream>

#include "BalancerDefines.h"
<<<<<<< HEAD

Ultrasoon_controller::Ultrasoon_controller()
{
	mFront = new Ultrasoon( ULTRASOON_F_TRIGGER, ULTRASOON_F_ECHO, ULTRASOON_F_TYPE );
	mBack = new Ultrasoon( ULTRASOON_B_TRIGGER, ULTRASOON_B_ECHO, ULTRASOON_B_TYPE );
=======
#include <generic/Logger.h>

Ultrasoon_controller::Ultrasoon_controller()
{
	mFront = new Ultrasoon( ULTRASOON_F_TRIGGER, ULTRASOON_F_ECHO );
	mBack = new Ultrasoon( ULTRASOON_B_TRIGGER, ULTRASOON_B_ECHO );
>>>>>>> master
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
<<<<<<< HEAD
	std::cerr << "Initialize ultrasoon" << std::endl;
=======
	//std::cerr << "Initialize ultrasoon" << std::endl;
	if( !mFront->isInitialized() ) {
		ERR("Could not initialize front Ultrasoon");
		stop();
	}
	if( !mBack->isInitialized() ) {
		ERR("Could not initialize back Ultrasoon");
		stop();
	}
>>>>>>> master
}

void Ultrasoon_controller::onRun()
{
<<<<<<< HEAD
	std::cerr << "read ultrasoon" << std::endl;
	std::cerr << "process data" << std::endl;
	std::cerr << "store data" << std::endl;
=======
	//std::cerr << "read ultrasoon" << std::endl;
	//std::cerr << "process data" << std::endl;
	//std::cerr << "store data" << std::endl;
>>>>>>> master
}
