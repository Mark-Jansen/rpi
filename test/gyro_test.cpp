#include <stdint.h>
#include <iostream>
#include <generic/File.h>
#include <gyro/gyro.h>


static int kNumSamples = 4000;

// milliseconds
int tickCount()
{
	struct timespec ts;
	clock_gettime( CLOCK_MONOTONIC, &ts );
	int value = (static_cast<int>(ts.tv_sec) * 1000) + (static_cast<int>(ts.tv_nsec) / 1000000);
	return value ? value : 1;
}


int main()
{
	File sensor( "/dev/gyro", O_RDWR );
	gyro_data data;
	int good = 0;
	std::cout << "Sample sensor " << kNumSamples << " times" << std::endl;
	int start = tickCount();
	int test = tickCount();
	std::cout << "Tickcount overhead: " << (test-start) << "ms" << std::endl;
	start = tickCount();
	for( int n = 0; n < kNumSamples; ++n ) {
		if( sensor.ioctl( GYRO_GET_DATA, data ) ) {
			++good;
		}
	}
	int end = tickCount();
	std::cout << "Succeeded: " << good << std::endl;
	std::cout << "Took " << (end-start) << "ms" << std::endl;

	std::cout << "Last sample (raw data):" << std::endl;
	std::cout << "Accel: " << data.accel_x << ", " << data.accel_y << ", " << data.accel_z << std::endl;
	std::cout << "Temperatur: " << data.raw_temperature << std::endl;
	std::cout << "Gyro: " << data.gyro_x << ", " << data.gyro_y << ", " << data.gyro_z << std::endl;

	return 0;
}
