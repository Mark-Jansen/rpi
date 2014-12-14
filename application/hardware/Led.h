#ifndef HARDWARE_LED_H
#define HARDWARE_LED_H

#include <generic/File.h>

class Led
{
public:
	Led(int pinNr);
	Led(int pinNr, int blinkTimer);
	~Led();
	void on(void);
	void off(void);
	void blink(void);
	int getStatus(void);
	int getBlinktimer(void);
	void setBlinktimer(int blinkTimer);

private:
	File mSensor;
	int status; //led is on, off or blinking
	int pinNr;
	int blinkTimer; 
};

#endif // HARDWARE_LED_H
