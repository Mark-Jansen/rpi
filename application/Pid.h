#ifndef APPLICATION_PID_H
#define APPLICATION_PID_H


class Pid
{
public:
	Pid(double kp, double ki, double kd);
	~Pid();
	int calculate(double *input, double *setpoint); //return percentage 0..100% to set motor pwm
	double getP();
	double getI();
	double getD();
    void setP(double p);
    void setI(double i);
    void setD(double d);
    void resetError();
	
private:
    double input;
	double setpoint;
	double Kp;
	double Ki;
    double Kd;
	double error;
};


#endif //  APPLICATION_PID_H

/* Classic PID
 *
 *    The Ziegler–Nichols tuning method is as follows;
 *
 *    The P, I & D terms are referred to as “gains” Kp, Ki & Kd.
 *    The Ki and Kd terms are first set to zero.
 *    The Kp term is manually increased until the robot reaches the ultimate gain, Ku, at which point the it starts to oscillate.
 *    The oscillation period Pu is measured and used to set the PID terms as follows :
 *    Kp = 0.6 * Ku
 *    Ki = (2 * Kp) / Pu
 *    Kd = (Kp * Pu) / 8
 */
 
 /* explanation
  * input  -> tilt of robot (IMU)
  * output -> speed (pwm) of motors
  */
  
