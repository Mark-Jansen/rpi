#include "hardware/Motor.h"
#include "MotorController.h"

#include <iostream>

#include "BalancerDefines.h"
#include <generic/Logger.h>


MotorController::MotorController()
{
	Motor::Config cfg( MOTOR_L_PWM, MOTOR_L_FORWARD, MOTOR_L_REVERSE, MOTOR_L_ENCODER_1, MOTOR_L_ENCODER_2 );
	mLeft = new Motor( cfg );
	cfg = Motor::Config( MOTOR_R_PWM, MOTOR_R_FORWARD, MOTOR_R_REVERSE, MOTOR_R_ENCODER_1, MOTOR_R_ENCODER_2 );
	mRight = new Motor( cfg );
}

MotorController::~MotorController()
{
	delete mLeft;
	delete mRight;
}

void MotorController::setSpeed( int speed )
{
	//mLeft->setSpeed( speed );
	//mRight->setSpeed( speed );
}

void MotorController::setDirection( Direction dir )
{
	switch( dir ) {
	case Direction::Forward:
		std::cerr << "Moving forward" << std::endl;
		//mLeft->setDirection(Forward);
		//mRight->setDirection(Forward);
		break;
	case Direction::Backward:
		std::cerr << "Moving backward" << std::endl;
		//mLeft->setDirection(Backward);
		//mRight->setDirection(Backward);
		break;
	case Direction::None:
		std::cerr << "Balancing" << std::endl;
		//mLeft->setDirection(None);
		//mRight->setDirection(None);
		break;
	}
}
