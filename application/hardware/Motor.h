#ifndef APPLICATION_MOTOR_H
#define APPLICATION_MOTOR_H

#include <generic/File.h>

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
    void setDirection(int direction);
	int  getDirection(void);
	int  readEncoder(void); //TODO
	void turnOff(void);
	void turnOn(void);

private:
	File mSensor;
	Config mConfig;
	int mSpeed; //TODO: translate to dutyCycle in code
	int mEnabled;
};

#endif // APPLICATION_MOTOR_H
