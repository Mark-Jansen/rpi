#ifndef HARDWARE_BATTERY_H
#define HARDWARE_BATTERY_H

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

#endif // HARDWARE_BATTERY_H


