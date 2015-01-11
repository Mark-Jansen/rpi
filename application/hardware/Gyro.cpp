#define _GLIBCXX_USE_NANOSLEEP		// otherwise we can't use sleep_for...
#include "Gyro.h"

#include <cmath>
#include <iostream>
#include <iomanip>

#include <generic/Logger.h>
#include <gyro/gyro.h>


static const float kGyroSensitivity = 131;
static int kNumCalibrationSamples = 10;

static float kRad2Deg = 180 / M_PI;


// TODO: move this to some generic util lib
int tickCount()
{
	struct timespec ts;
	clock_gettime( CLOCK_MONOTONIC, &ts );
	int value = (static_cast<int>(ts.tv_sec) * 1000) + (static_cast<int>(ts.tv_nsec) / 1000000);
	return value ? value : 1;
}


Gyro::Gyro()
: mSensor( GYRO_DEVICE, O_RDWR )
{
	reset();
}

Gyro::~Gyro()
{
	stop();
}

void Gyro::reset()
{
	mInitialGyro = Vec3f();
	mInitialAccel = Vec3f();

	mLastAngle = Vec3f();
	mLastTemperature = 0;
	mLastAngleTick = 0;
}

int Gyro::delayMS() const
{
	return 20;
}

void Gyro::calibrate()
{
	reset();
	gyro_data data;
	int good_samples = 0;
	mSensor.ioctl( GYRO_GET_DATA, data );		// discard the first sample
	for( int n = 0; n < kNumCalibrationSamples; ++n ) {
		if( mSensor.ioctl( GYRO_GET_DATA, data ) ) {
			++good_samples;
			mInitialGyro += Vec3f( data.gyro_x, data.gyro_y, data.gyro_z );
			mInitialAccel += raw2Accel( data.accel_x, data.accel_y, data.accel_z );
		}
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
	if( good_samples != kNumCalibrationSamples ) {
		ERR("Problem calibrating!");
	}
	mInitialGyro /= good_samples;
	mInitialAccel /= good_samples;
}

void Gyro::onBeforeRun()
{
	if( !mSensor.isOpen() ) {
		ERR("Sensor not opened...");
		stop();
		return;
	}
	if( !mLastAngleTick ) {
		calibrate();
		mLastAngleTick = tickCount();
	}
}

Vec3f Gyro::raw2Accel( float xout, float yout, float zout )
{
	float roll = atan2(yout, zout) * kRad2Deg;
	float pitch = atan2(xout, sqrt(yout*yout + zout*zout)) * 180 / M_PI;
	return Vec3f( roll, pitch, 0 );
}

// complementary filter.
void Gyro::onRun()
{
	gyro_data data;
	if( mSensor.ioctl( GYRO_GET_DATA, data ) ) {

		Vec3f accel = raw2Accel( data.accel_x, data.accel_y, data.accel_z ) - mInitialAccel;
		//std::cout << "Accel: " << std::setw(10) << accel.x << ", " << std::setw(10) << accel.y;

		mLastTemperature = data.raw_temperature / 340.f + 36.53f;
		Vec3f rawGyro = Vec3f( data.gyro_x, data.gyro_y, data.gyro_z ) - mInitialGyro;
		rawGyro /= kGyroSensitivity;

		int tick = tickCount();
		
		float dt = (tick - mLastAngleTick) / 1000.0f;
		Vec3f gyro = rawGyro * dt + mLastAngle;

		//std::cout << ", Gyro: " << std::setw(10) << gyro.x << ", " << std::setw(10) << gyro.y << ", " << std::setw(10) << gyro.z;

		float alpha = 0.96;
		Vec3f angle = gyro * alpha + accel * (1.f - alpha);

		LOG("Result: " << std::setw(10) << angle.x << ", " << std::setw(10) << angle.y);

		mLastAngle = angle;
		mLastAngleTick = tick;
	}
}
