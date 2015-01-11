#ifndef CONTROLLER_H
#define CONTROLLER_H

class Battery;
class Button;
class Gyro;
class Led;
class MotorController;
class Ultrasoon_controller;

class Controller
{
public:
	Controller();
	~Controller();
	int run();

private:
	void init();
	void deinit();
	bool runFrame();

	Ultrasoon_controller* mUltrasoon;
	Gyro* mGyro;
	Led* mBatteryLed;
	Led* mReadyLed;
	Button* mPowerButton;
	Battery* mBattery;
	MotorController* mMotorController;
};


#endif // CONTROLLER_H
