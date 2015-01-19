#include "Controller.h"

#include <iostream>

#include "Ultrasoon_controller.h"
#include "MotorController.h"

#include "hardware/Gyro.h"
#include "hardware/Battery.h"
#include "hardware/Led.h"
#include "hardware/Button.h"
#include <generic/Logger.h>

#include "BalancerDefines.h"

Controller::Controller()
{
	mUltrasoon = new Ultrasoon_controller;
	mGyro = new Gyro;
	mBatteryLed = new Led( LED_PIN_POWER );
	mReadyLed = new Led( LED_PIN_READY );
	mPowerButton = new Button( BUTTON_PIN_POWER );
	mBattery = new Battery( *mBatteryLed );
	mMotorController = new MotorController();
}

Controller::~Controller()
{
	delete mMotorController;
	delete mBattery;
	delete mBatteryLed;
	delete mReadyLed;
	delete mPowerButton;
	delete mGyro;
	delete mUltrasoon;
}

void Controller::init()
{
	mPowerButton->start();
	mBattery->start();
	mGyro->start();
	mUltrasoon->start();
}

void Controller::deinit()
{
	mUltrasoon->stop();
	mGyro->stop();
	mBattery->stop();
	mPowerButton->stop();
}

int Controller::run()
{
	init();
	while( !mPowerButton->wasPressed() ) {
		if( !runFrame() ) {
			ERR("runFrame abort");
		}
	}
	deinit();
	return 0;
}

bool Controller::runFrame()
{
	//std::cerr << "read gyro" << std::endl;
	//std::cerr << "read ultrasoon" << std::endl;
	//std::cerr << "process data" << std::endl;
	//std::cerr << "output to motor controller" << std::endl;
	return true;
}
