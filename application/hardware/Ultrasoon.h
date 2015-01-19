#ifndef HARDWARE_ULTRASOON_H
#define HARDWARE_ULTRASOON_H

#include <generic/File.h>

class Ultrasoon
{
public:
	Ultrasoon(int triggerPin, int echoPin, int type); 
	virtual ~Ultrasoon();
	int getDistance(void);
	bool isInitialized() const;

private:
	File mSensor;
	int mTriggerPin;
	int mEchoPin;
	int mType;	
};

#endif // HARDWARE_ULTRASOON_H

