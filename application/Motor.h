#ifndef APPLICATION_MOTOR_H
#define APPLICATION_MOTOR_H

#include "generic/File.h"

class Motor
{
public:
	Motor(int pwmPin, int dirPinForward, int dirPinReverse, int encoderPin_1, int encoderPin_2);
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
	int pwmPin;
	int dirPinForward;
	int dirPinReverse;
    int encoderPin_1;
	int encoderPin_2;
	int speed; //TODO: translate to dutyCycle in code
	int enabled;
};

#endif // APPLICATION_MOTOR_H
