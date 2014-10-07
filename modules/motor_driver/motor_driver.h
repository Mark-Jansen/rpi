#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <linux/ioctl.h>


struct motor_driver_data {
	
	int pwm_pin;
	int pwm;
	int direction_pin1;
	int direction_pin2;
	
};

struct encoder_info {
	int motor_speed;
	int direction;
};

#define THERMIOC_MAGIC			'M'
#define MOTOR_SETSPEED			_IOW(THERMIOC_MAGIC, 0, struct motor_driver_data)
#define MOTOR_GETSPEED			_IOR(THERMIOC_MAGIC, 1, encoder_info)

// =================================================================================
// void setSpeed(struct motor_driver_data* arg)
// Pre : 
// Post: sets the speed of the motor
// =================================================================================

// =================================================================================
// int getSpeed(struct encoder_info* arg)
// Pre : 
// Post: returns the speed of the motor
// =================================================================================


#endif // MOTOR_DRIVER_H