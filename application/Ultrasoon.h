#ifndef APPLICATION_ULTRASOON_H
#define APPLICATION_ULTRASOON_H

#include "generic/File.h"
#include "generic/Thread.h"


class Ultrasoon : public Thread
{  
public:
	Ultrasoon(int triggerPin, int echoPin, int type); 
	virtual ~Ultrasoon();
	void calibrate();

protected:
	int delayMS() const;
	virtual void onBeforeRun();
	virtual void onRun();


private:
	File mSensor;
	int distance;
	int triggerPin;
	int echoPin;
    int type; //0= Front sensor , 1= Back sensor
};

#endif // APPLICATION_ULTRASOON_H

