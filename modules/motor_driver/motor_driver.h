#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <linux/ioctl.h>

struct motor_driver_config{
	int pinnr;
	int direction_left_pinnr;
	int direction_right_pinnr;
};

struct motor_driver_setting {
	int pwm;
	int direction_pinL;   // 0 or 1 to turn left or right
	int direction_pinR;	  // 0 or 1 to turn left or right
	
};

struct motor_driver_encoder_data {
	int speed;
	int direction;      // 0 is left , 1 is right
};

#define THERMIOC_MAGIC				'M'
#define MOTOR_DRIVER_SET_CONFIG 	_IOW(THERMIOC_MAGIC, 0, struct motor_driver_config)
#define MOTOR_SETSPEED				_IOW(THERMIOC_MAGIC, 1, struct motor_driver_setting)
#define MOTOR_GETSPEED				_IOR(THERMIOC_MAGIC, 2, motor_driver_encoder_data)


// =================================================================================
// int motor_driver_config(struct motor_driver_config* arg)
// Pre : 
// Post: sets pinnumbers for use in RPI
// =================================================================================
int motor_driver_config(struct motor_driver_config* arg)

// =================================================================================
// int setSpeed(struct motor_driver_setting* arg)
// Pre : 
// Post: sets the speed(pwm)and direction of the motor
// =================================================================================
int setSpeed(struct motor_driver_setting* arg)

// =================================================================================
// int getSpeed(struct motor_driver_encoder_data* arg)
// Pre : 
// Post: returns the speed and direction of the motor
// =================================================================================
int getSpeed(struct motor_driver_encoder_data* arg)

#endif // MOTOR_DRIVER_H