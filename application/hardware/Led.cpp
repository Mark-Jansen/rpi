#include "Led.h"

Led::Led(int pinNr)
	: mSensor( "/dev/led", O_RDWR )
{  
}

Led::~Led() 
{
}

void Led::on(void)
{
}

void Led::off(void)
{
}

void Led::blink(void)
{
}

int Led::getStatus(void)
{
}

int Led::getBlinktimer(void)
{
}

void Led::setBlinktimer(int blinkTimer)
{
}

