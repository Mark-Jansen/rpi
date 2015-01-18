#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#define SW_PWM	1
#define HW_PWM  0

struct motor_driver_setting {
	int direction_in1_pinnr;
	int direction_in2_pinnr;
	int direction_pinL;   // 0 or 1 to turn left or right
	int direction_pinR;	  // 0 or 1 to turn left or right
	int pwm_channel;     //use SW_PWM for software pwm, HW_PWM for hardware pwm.
	int pwm_pinnr;
	int pwm_enable;
	int pwm_frequency;
	int pwm_duty_cycle;
	int speed;
	int direction;      // 0 is left , 1 is right
};

#define THERMIOC_MAGIC_MOTOR_DRIVER 	'M'
#define MOTOR_DRIVER_SET_SETTINGS 					_IOW(THERMIOC_MAGIC_MOTOR_DRIVER, 0, struct motor_driver_setting)
#define MOTOR_DRIVER_GET_SETTINGS					_IOR(THERMIOC_MAGIC_MOTOR_DRIVER, 1, struct motor_driver_setting)
#define MOTOR_DRIVER_SET_SPEED						_IOW(THERMIOC_MAGIC_MOTOR_DRIVER, 2, struct motor_driver_setting)
#define MOTOR_DRIVER_GET_SPEED						_IOR(THERMIOC_MAGIC_MOTOR_DRIVER, 3, struct motor_driver_setting)
#define MOTOR_DRIVER_SET_DIRECTION					_IOW(THERMIOC_MAGIC_MOTOR_DRIVER, 4, struct motor_driver_setting)
#define MOTOR_DRIVER_GET_DIRECTION					_IOR(THERMIOC_MAGIC_MOTOR_DRIVER, 5, struct motor_driver_setting)

// =================================================================================
// int motor_driver_set_settings(struct motor_driver_setting* arg)
// Pre : 
// Post: set motor_driver settings
// =================================================================================
int motor_driver_set_settings(struct motor_driver_setting* arg);

// =================================================================================
// int motor_driver_get_settings(struct motor_driver_setting* arg)
// Pre : 
// Post: sets pinnumbers for use in RPI
// =================================================================================
int motor_driver_get_settings(struct motor_driver_setting* arg);

// =================================================================================
//  EXPORTED SYMBOLS
// =================================================================================
int setSpeed(struct motor_driver_setting* arg);
int getSpeed(struct motor_driver_setting* arg);
int setDirection(struct motor_driver_setting* arg); 
int getDirection(struct motor_driver_setting* arg);

#endif // MOTOR_DRIVER_H