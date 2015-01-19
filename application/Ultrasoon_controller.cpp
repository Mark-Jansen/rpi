#include "Ultrasoon_controller.h"
#include <hardware/Ultrasoon.h>
#include <iostream>

#include "BalancerDefines.h"
#include <generic/Logger.h>

Ultrasoon_controller::Ultrasoon_controller()
{
	LOG("Front and Back sensor initializing.");
	mBack = new Ultrasoon( ULTRASOON_B_TRIGGER, ULTRASOON_B_ECHO,0 );
	mFront = new Ultrasoon( ULTRASOON_F_TRIGGER, ULTRASOON_F_ECHO,1 );
}

Ultrasoon_controller::~Ultrasoon_controller() 
{
	delete mFront;
	delete mBack;
}

int Ultrasoon_controller::delayMS() const
{
	return 50;	// nader te bepalen tijdelijk 2sec voor geen spam tijden testen
}

void Ultrasoon_controller::onBeforeRun()
{
	//std::cerr << "Initialize ultrasoon" << std::endl;
	
	if( !mFront->isInitialized() ) {
		ERR("Could not initialize front Ultrasoon");
		stop();
	}
	LOG("Front sensor ready.");
	
	if( !mBack->isInitialized() ) {
		ERR("Could not initialize back Ultrasoon");
		stop();
	}
	LOG("Back sensor ready.");
	
}

void Ultrasoon_controller::onRun()
{
	int distaneValue = 0;
	//get distance
	distaneValue = mFront->getDistance();
	
	//Print data
	std::cerr << "Ultrasoon	distance front: " ;
	if(	distaneValue > 0)
		std::cerr << distaneValue << "cm"<<std::endl ;
	else
		std::cerr << "unusable  data" <<std::endl ;
	
	//get distance
	distaneValue = mBack->getDistance();
	
	//print data
	std::cerr << "Ultrasoon	distance back: " ;
	if(	distaneValue > 0)
		std::cerr << distaneValue << "cm"<<std::endl ;
	else
		std::cerr << "unusable  data" <<std::endl ;

}












