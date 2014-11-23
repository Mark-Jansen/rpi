#include <stdio.h>
#include <stdint.h>
#include <string>
#include "Gyro.h"

int main()
{
	Gyro gyro;
	gyro.start();
	sleep(40);
	gyro.stop();
	return 0;
}
