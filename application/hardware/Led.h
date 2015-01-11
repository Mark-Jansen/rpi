#ifndef HARDWARE_LED_H
#define HARDWARE_LED_H

#include <generic/File.h>

class Led
{
public:
	Led(int pinNr, int blinkTimer = 0);
	~Led();
	void on(void);
	void off(void);
	void blink(void);
	int getStatus(void);
	int getBlinktimer(void);
	void setBlinktimer(int blinkTimer);

private:
	void write( int ioctl, int value );

	File mSensor;
	int mPinNr;
	int mBlinkTimer;
};

#endif // HARDWARE_LED_H
