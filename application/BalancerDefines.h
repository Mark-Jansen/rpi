#ifndef BALANCERDEFINES_H
#define BALANCERDEFINES_H

// maybe move the device names to their own headers?

#define GPIO_DEVICE				"/dev/gpio"


#define BUTTON_PIN_POWER		100


#define LED_DEVICE				"/dev/led"

#define LED_PIN_POWER			1
#define LED_PIN_READY			99


#define BATTERY_DEVICE			"/dev/battery"

#define BATTERY_CHECK_SECONDS	5
#define BATTERY_BLINK_LEVEL		15
#define BATTERY_WARN_LEVEL		10
#define BATTERY_SHUTDOWN_LEVEL	5


#define MOTOR_DEVICE			"/dev/motor"

#define MOTOR_L_PWM				2
#define MOTOR_L_FORWARD			3
#define MOTOR_L_REVERSE			4
#define MOTOR_L_ENCODER_1		5
#define MOTOR_L_ENCODER_2		6

#define MOTOR_R_PWM				7
#define MOTOR_R_FORWARD			8
#define MOTOR_R_REVERSE			9
#define MOTOR_R_ENCODER_1		10
#define MOTOR_R_ENCODER_2		11


#define ULTRASOON_DEVICE		"/dev/ultrasoon"

#define ULTRASOON_F_TRIGGER		120
#define ULTRASOON_F_ECHO		121
#define ULTRASOON_F_TYPE		0

#define ULTRASOON_B_TRIGGER		130
#define ULTRASOON_B_ECHO		141
#define ULTRASOON_B_TYPE		1

#endif // BALANCERDEFINES_H
