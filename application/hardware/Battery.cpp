#include "Battery.h"
#include "Led.h"
#include "BalancerDefines.h"
#include <battery/battery.h>

Battery::Battery( Led& warningLed )
	: mSensor( BATTERY_DEVICE, O_RDWR )
	, mWarningLed( warningLed )
{
}

Battery::~Battery() 
{
}

int Battery::delayMS() const
{
	return BATTERY_CHECK_SECONDS * 1000;
}

void Battery::onBeforeRun()
{
	mWarningLed.off();
}

void Battery::onRun()
{
	battery_charge charge = {0};
	if( !mSensor.ioctl( BATTERY_GET_CHARGE, charge ) ) {
		mWarningLed.blink();
	}
	int least = std::min( charge.level1, charge.level2 );
	if( least <= BATTERY_SHUTDOWN_LEVEL ) {
		// ask the controller to stop.
	} else if( least <= BATTERY_WARN_LEVEL ) {
		mWarningLed.blink();
	} else if( least <= BATTERY_BLINK_LEVEL ) {
		mWarningLed.on();
	} else {
		mWarningLed.off();
	}
}
