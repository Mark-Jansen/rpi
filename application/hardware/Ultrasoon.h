#ifndef APPLICATION_ULTRASOON_H
#define APPLICATION_ULTRASOON_H

#include <generic/File.h>

class Ultrasoon
{
public:
	Ultrasoon(int triggerPin, int echoPin, int type); 
	virtual ~Ultrasoon();
	int getDistance(void);

private:
	File mSensor;
	int distance; 
	int triggerPin;
	int echoPin;
	int type; //0= Front sensor , 1= Back sensor
};

#endif // APPLICATION_ULTRASOON_H

