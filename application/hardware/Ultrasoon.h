#ifndef HARDWARE_ULTRASOON_H
#define HARDWARE_ULTRASOON_H

#include <generic/File.h>

class Ultrasoon
{
public:
<<<<<<< HEAD
	Ultrasoon(int triggerPin, int echoPin, int type); 
	virtual ~Ultrasoon();
	int getDistance(void);
=======
	Ultrasoon(int triggerPin, int echoPin); 
	virtual ~Ultrasoon();
	int getDistance(void);
	bool isInitialized() const;
>>>>>>> master

private:
	File mSensor;
	int mTriggerPin;
	int mEchoPin;
<<<<<<< HEAD
	int mType; //0= Front sensor , 1= Back sensor
=======
>>>>>>> master
};

#endif // HARDWARE_ULTRASOON_H

