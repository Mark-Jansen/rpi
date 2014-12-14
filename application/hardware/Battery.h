#ifndef APPLICATION_BATTERY_H
#define APPLICATION_BATTERY_H

#include <generic/File.h>
#include <generic/Thread.h>
#include "Led.h"

class Battery : public Thread
{
public:

	Battery( Led& warningLed );
	virtual ~Battery();

protected:

	int delayMS() const;
	virtual void onBeforeRun();
	virtual void onRun();


private:
	File mSensor;
	Led& mWarningLed;
};

#endif // APPLICATION_BATTERY_H


