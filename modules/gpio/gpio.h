#ifndef GPIO_H
#define GPIO_H

/* I/O settings */ 
struct gpio_status {
	int pinNr;		// Number of GPIO pin
	int value;		// when fuction = output: use this value to write. when function = input: this value is actual value of pin
	int function;	// use defines (INPUT, OUTPUT, ALT0, ALT1, ALT2, ALT3, ALT4 or ALT5)
};

/* Gpio registers: check datasheet for more information */
struct GpioRegisters
{
	// function select
	int GPFSEL[6];		// GPIO FUNCTION SELECT:	use this register to set the function to input, output or alternative
	int Reserved1;
	// output
	int GPSET[2];		// GPIO SET: 				Use this register to set outputvalue to 1	
	int Reserved2;
	int GPCLR[2];		// GPIO CLEAR: 				Use this register to set outputvalue to 0	
	int Reserved3;
	// input
	int GPLEV[2];		// GPIO LEVEL:				Use this register to read the actuel value of the pin 
	int Reserved4;
};

/* function: check datasheet for pin function */
#define 	INPUT				0b000		// check datasheet
#define		OUTPUT				0b001		// check datasheet
#define		ALT0				0b100		// alternate function 0 (pinNr 12 = PWM0, pinNr13 = PWM1, pinNr40 = PWM0, pinNr41 = PWM1, pinNr45 = PWM1)
#define		ALT1				0b101		// alternate function 1
#define		ALT2				0b110		// alternate function 2
#define		ALT3				0b111		// alternate function 3
#define		ALT4				0b011		// alternate function 4
#define		ALT5				0b010		// alternate function 5 (pinNr 18 = PWM0, pinNr19 = PWM1)

#define THERMIOC_MAGIC_GPIO			'G'

#define GPIO_WRITE					_IOW(THERMIOC_MAGIC_GPIO, 0, struct gpio_status)
#define GPIO_READ					_IOR(THERMIOC_MAGIC_GPIO, 1, struct gpio_status)
#define GPIO_SET_CONFIG				_IOW(THERMIOC_MAGIC_GPIO, 2, struct gpio_status)

/* export symbols */
int gpio_write(struct gpio_status* arg);		// write a value to a gpiopin
int gpio_read(struct gpio_status* arg);			// read actual value of gpiopin
int gpio_set_config(struct gpio_status* arg);	// set function and value to a gpiopin

#endif // GPIO_H
