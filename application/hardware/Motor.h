#ifndef HARDWARE_MOTOR_H
#define HARDWARE_MOTOR_H

#include <generic/File.h>

enum class Dir //direction
{
	None,
	Forward,
	Backward
};

class Motor
{
public:

	struct Config
	{
		Config(int pwmPin, int dirPinForward, int dirPinReverse, int encoderPin_1, int encoderPin_2)
		: PwmPin(pwmPin), DirPinForward(dirPinForward), DirPinReverse( dirPinReverse ), EncoderPin1(encoderPin_1), EncoderPin2(encoderPin_2) {;}
		int PwmPin;
		int DirPinForward;
		int DirPinReverse;
		int EncoderPin1;
		int EncoderPin2;
	};

	Motor(const Config& cfg);
	~Motor();
	void setSpeed(int speed);
	int  getSpeed(void);
    void setDirection(Dir d);
	Dir getDirection(void);
	int  readEncoder(void); //TODO
	void turnOff(void);
	void turnOn(void);

private:
	File mSensor;
	Config mConfig;
	int mSpeed; //TODO: translate to dutyCycle in code
	int mEnabled;
};

#endif // HARDWARE_MOTOR_H
