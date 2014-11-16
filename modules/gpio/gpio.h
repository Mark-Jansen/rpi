#ifndef GPIO_H
#define GPIO_H

/* I/O settings */ 
struct gpio_status {
	int pinNr;		// Number of GPIO pin
	int value;		// when fuction = output: use this value to write. when function = input: this value is actuel value of pin
	int function;	// use defines
};

/* Gpio registers: check datasheet for more information */
struct GpioRegisters
{
	// function select
	int GPFSEL[6];		// GPIO SELECT:	Deze kun je gebruiken om een gekozen pin input of output te maken	// check datasheet
	int Reserved1;
	// output
	int GPSET[2];		// GPIO SET: 	Deze kun je gebruiken om de gekozen pin 1 te maken					// check datasheet
	int Reserved2;
	int GPCLR[2];		// GPIO CLEAR: 	Deze kun je gebruiken om de gekozen pin 0 te maken					// check datasheet
	int Reserved3;
	// input
	int GPLEV[2];		// GPIO LEVEL:	Deze geeft de actuele waarden van de GPIO pin
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

#define THERMIOC_MAGIC			'G'

#define GPIO_WRITE			_IOW(THERMIOC_MAGIC, 0, struct gpio_status)
#define GPIO_READ			_IOR(THERMIOC_MAGIC, 1, struct gpio_status)
#define GPIO_SET_CONFIG		_IOW(THERMIOC_MAGIC, 2, struct gpio_status)

/* export symbols */
int gpio_write(struct gpio_status* arg);
int gpio_read(struct gpio_status* arg);
int gpio_set_config(struct gpio_status* arg);

#endif // GPIO_H
