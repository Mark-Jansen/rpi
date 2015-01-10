#include <iostream>
#include <unistd.h>
#include "Gyro.h"

using std::cout;
using std::endl;

int main()
{
	Gyro gyro;
	cout << "Sampling gyro" << endl;
	gyro.start();
	sleep(40);
	gyro.stop();

	return 0;
}
