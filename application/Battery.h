#ifndef APPLICATION_BATTERY_H
#define APPLICATION_BATTERY_H

#include "generic/File.h"
#include "generic/Thread.h"

class Battery : public Thread
{
public:

	Battery(void);
	virtual ~Battery();

protected:

	int delayMS() const;
	virtual void onBeforeRun();
	virtual void onRun();


private:	
	File mSensor;
};

#endif // APPLICATION_BATTERY_H


