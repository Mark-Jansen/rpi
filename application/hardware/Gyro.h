#ifndef APPLICATION_GYRO_H
#define APPLICATION_GYRO_H

#include <generic/File.h>
#include "generic/Thread.h"

class Gyro : public Thread
{
public:
	Gyro();
	virtual ~Gyro();

	void calibrate();

protected:

	int delayMS() const;
	virtual void onBeforeRun();
	virtual void onRun();

private:
	File mSensor;
	int mXOffset, mYOffset, mZOffset;
};

#endif // APPLICATION_GYRO_H
