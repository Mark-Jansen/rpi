#include "Button.h"
#include <gpio/gpio.h>
#include <iostream>
#include "BalancerDefines.h"
#include <generic/Logger.h>

Button::Button( int pin )
	: mSensor( GPIO_DEVICE, O_RDWR )
	, mPin( pin )
	, mDown( false )
	, mWasDown( false )
{
}

int Button::delayMS() const
{
	return mDown ? 20 : 100;
}

bool Button::isDown()
{
	gpio_status gs = {0};
	gs.pinNr = mPin;
	if( mSensor.ioctl( GPIO_READ, gs ) ) {
		return gs.value != 0;
	}
	// log it once??
	return false;
}

bool Button::wasPressed()
{
	bool old = mWasDown;
	mWasDown = false;
	return old;
}

void Button::onBeforeRun()
{
	gpio_status gs = {0};
	gs.pinNr = mPin;
	gs.function = INPUT;
	if( !mSensor.ioctl( GPIO_SET_CONFIG, gs ) ) {
		ERR("Could not initialize button on pin " << mPin);
		stop();
	}
}

void Button::onRun()
{
	bool down = isDown();
	mDown |= down;
	if( !down && mDown ) {
		mWasDown = true;
		mDown = false;
	}
}

