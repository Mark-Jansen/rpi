#ifndef HARDWARE_ULTRASOON_H
#define HARDWARE_ULTRASOON_H

#include <generic/File.h>

class Ultrasoon
{
public:
	Ultrasoon(int triggerPin, int echoPin, int type); 
	virtual ~Ultrasoon();
	int getDistance(void);

private:
	File mSensor;
	int mTriggerPin;
	int mEchoPin;
	int mType; //0= Front sensor , 1= Back sensor
};

#endif // HARDWARE_ULTRASOON_H

