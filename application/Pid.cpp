#include "Pid.h"

Pid::Pid(double kp, double ki, double kd)
{
	Kp = kp; //8
	Ki = ki; //2
	Kd = kd; //9
	error = 0;
}

Pid::~Pid() 
{
	//TODO
}

int Pid::calculate(double *input, double *setpoint) //return percentage 0..100% to set motor pwm, input and setpoint are angles
{
	double lastError = getError();
	double iTerm;
	/* Update PID values */
	double error = (*setpoint - *input)/100;
	double pTerm = Kp * error;
	iTerm += Ki * error;
	double dTerm = Kd * (error - lastError);
	lastError = error;
	double PIDValue = pTerm + iTerm + dTerm;

	//PIDLeft = PIDValue;
	//PIDRight = PIDValue;

	//PIDLeft *= 0.95; // compensate for difference in the motors

	/* Set PWM Values */
	/*
	if (PIDLeft >= 0)
	move(left, forward, PIDLeft);
	else
	move(left, backward, PIDLeft * -1);
	if (PIDRight >= 0)
	move(right, forward, PIDRight);
	else
	move(right, backward, PIDRight * -1);
	*/
	if (PIDValue >= 0) return PIDValue; //moving forward
	else return PIDValue * -1;	//moving backwards
}

double Pid::getP()
{
	return Kp;
}

double Pid::getI()
{
	return Ki;
}

double Pid::getD()
{
	return Kd;
}

double Pid::getError()
{
	return error;
}

void Pid::setP(double p)
{
	Kp = p;
}    

void Pid::setI(double i)
{
	Ki = i;
}    

void Pid::setD(double d)
{
	Kd = d;
}    

void Pid::resetError()
{
	error = 0;
}

