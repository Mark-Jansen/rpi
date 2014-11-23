#include "Gyro.h"
#include <gyro/gyro.h>
#include <cmath>
#include <fcntl.h>
#include <iostream>
#include <iomanip>

static float kGyroXSensitivity = 66.5f;
static float kGyroYSensitivity = 66.5f;
static float kGyroZSensitivity = 65.5f;
static float kDt = 0.0025f;		// 1/400Hz
static int kNumCalibrationSamples = 500;

Gyro::Gyro()
: mSensor( "/dev/gyro", O_RDWR )
, mXOffset( 0 )
, mYOffset( 0 )
, mZOffset( 0 )
{
}

Gyro::~Gyro()
{
	stop();
}

int Gyro::delayMS() const
{
	return 100;
}

void Gyro::calibrate()
{
	signed long XOUT = 0, YOUT = 0, ZOUT = 0;
	gyro_data data;
	int good_samples = 0;
	for( int n = 0; n < kNumCalibrationSamples; ++n ) {
		if( mSensor.ioctl( GYRO_GET_DATA, data ) ) {
			++good_samples;
			XOUT += data.gyro_x;
			YOUT += data.gyro_y;
			ZOUT += data.gyro_z;
		}
	}
	if( good_samples != kNumCalibrationSamples ) {
		std::cout << "Problem calibrating!!" << std::endl;
	}
	mXOffset = XOUT / good_samples;
	mYOffset = YOUT / good_samples;
	mZOffset = ZOUT / good_samples;
	std::cout << "Calibration: x=" << mXOffset << ", y=" << mYOffset << ", z=" << mZOffset << std::endl;
}

void Gyro::onBeforeRun()
{
	if( !mSensor.isOpen() ) {
		std::cout << "Sensor not opened..." << std::endl;
		stop();
	}
	if( mXOffset == 0 && mYOffset == 0 && mZOffset == 0 ) {
		calibrate();
	}
}

void Gyro::onRun()
{
	gyro_data data;
	if( mSensor.ioctl( GYRO_GET_DATA, data ) ) {
	
		float xout = (float)data.accel_x;
		float yout = (float)data.accel_y;
		float zout = (float)data.accel_z;
		
		//float scalar = sqrt( xout * xout + yout * yout + zout * zout );
		//xout /= scalar;
		//yout /= scalar;
		//zout /= scalar;
		float roll = atan2(yout, zout) * 180 / M_PI;
		float pitch = atan2(xout, sqrt(yout*yout + zout*zout)) * 180 / M_PI;
		std::cout << "Accel: " << std::setw(10) << roll << ", " << std::setw(10) << pitch;

		std::cout << ", Temperature: " << std::setw(10) << data.raw_temperature / 340.f + 36.53f;
		float gyro_x = (data.gyro_x - mXOffset) / kGyroXSensitivity;
		float gyro_y = (data.gyro_y - mYOffset) / kGyroYSensitivity;
		float gyro_z = (data.gyro_z - mZOffset) / kGyroZSensitivity;
		std::cout << ", Gyro: " << std::setw(10) << gyro_x << ", " << std::setw(10) << gyro_y << ", " << std::setw(10) << gyro_z << std::endl;
	}
}
