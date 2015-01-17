#ifndef HARDWARE_GYRO_H
#define HARDWARE_GYRO_H

#include <generic/File.h>
#include <generic/Thread.h>
#include "generic/Vec3.h"
#include "generic/Spinlock.h"

struct Sample
{
	Vec3f Angle;
	int Tick;
};


class Gyro : public Thread
{
public:
	Gyro();
	virtual ~Gyro();
	
	Sample lastSample();

	void calibrate();

protected:

	void reset();

	int delayMS() const;
	virtual void onBeforeRun();
	virtual void onRun();

private:

	static Vec3f raw2Accel( float x, float y, float z );

	File mSensor;
	Vec3f mInitialGyro;
	Vec3f mInitialAccel;
	SpinLock mDataLock;
	Vec3f mLastAngle;
	float mLastTemperature;
	int mLastAngleTick;
};

#endif // HARDWARE_GYRO_H
