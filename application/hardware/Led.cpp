#include "Led.h"
#include <led/led.h>

#include <iostream>

#include <generic/Logger.h>
#include "BalancerDefines.h"

Led::Led(int pinNr, int blinkTimer)
	: mSensor( LED_DEVICE, O_RDWR )
	, mPinNr( pinNr )
	, mBlinkTimer( blinkTimer )
{
}

Led::~Led() 
{
}

void Led::on(void)
{
	write( LED_ON, ON );
}

void Led::off(void)
{
	write( LED_OFF, OFF );
}

void Led::blink(void)
{
	write( LED_BLINK, ON );
}

int Led::getStatus(void)
{
	// ??
}

int Led::getBlinktimer(void)
{
	return mBlinkTimer;
}

void Led::setBlinktimer(int blinkTimer)
{
	mBlinkTimer = blinkTimer;
	// write to led?
}

void Led::write( int ioctl, int value )
{
	led_status ls = {0};
	ls.pinNr = mPinNr;
	ls.blinkTimer = mBlinkTimer;
	ls.value = value;
	if( !mSensor.ioctl( ioctl, ls ) ) {
		ERR("Writing to led on pin " << mPinNr);
	}
}
