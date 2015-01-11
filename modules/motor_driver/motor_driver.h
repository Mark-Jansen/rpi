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
};

struct motor_driver_encoder_data {
	int speed;
	int direction;      // 0 is left , 1 is right
};

#define THERMIOC_MAGIC_MOTOR_DRIVER 	'M'
#define MOTOR_DRIVER_SET_CONFIG 		_IOW(THERMIOC_MAGIC_MOTOR_DRIVER, 0, struct motor_driver_setting)
#define MOTOR_SETSPEED					_IOW(THERMIOC_MAGIC_MOTOR_DRIVER, 1, struct motor_driver_setting)
#define MOTOR_GETSPEED					_IOR(THERMIOC_MAGIC_MOTOR_DRIVER, 2, struct motor_driver_encoder_data)


// =================================================================================
// int motor_driver_set_config(struct motor_driver_setting* arg)
// Pre : 
// Post: sets pinnumbers for use in RPI
// =================================================================================
int motor_driver_set_config(struct motor_driver_setting* arg);

// =================================================================================
// int setSpeed(struct motor_driver_setting* arg)
// Pre : 
// Post: sets the speed(pwm)and direction of the motor
// =================================================================================
int setSpeed(struct motor_driver_setting* arg);

// =================================================================================
// int getSpeed(struct motor_driver_encoder_data* arg)
// Pre : 
// Post: returns the speed and direction of the motor
// =================================================================================
int getSpeed(struct motor_driver_encoder_data* arg);

#endif // MOTOR_DRIVER_H