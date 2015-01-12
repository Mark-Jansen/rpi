#ifndef BALANCERDEFINES_H
#define BALANCERDEFINES_H

// maybe move the device names to their own headers?



#define BUTTON_PIN_POWER		11



#define LED_PIN_POWER			25
#define LED_PIN_READY			24


#define BATTERY_CHECK_SECONDS	5
#define BATTERY_BLINK_LEVEL		15
#define BATTERY_WARN_LEVEL		10
#define BATTERY_SHUTDOWN_LEVEL	5


#define MOTOR_L_PWM				18
#define MOTOR_L_FORWARD			4
#define MOTOR_L_REVERSE			17
#define MOTOR_L_ENCODER_1		10
#define MOTOR_L_ENCODER_2		9

#define MOTOR_R_PWM				23
#define MOTOR_R_FORWARD			27
#define MOTOR_R_REVERSE			22
#define MOTOR_R_ENCODER_1		8
#define MOTOR_R_ENCODER_2		7


#define ULTRASOON_F_TRIGGER		28
#define ULTRASOON_F_ECHO		30

#define ULTRASOON_B_TRIGGER		29
#define ULTRASOON_B_ECHO		31

#endif // BALANCERDEFINES_H
