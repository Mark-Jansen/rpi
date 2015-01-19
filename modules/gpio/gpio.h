#ifndef GPIO_H
#define GPIO_H

#define GPIO_DEVICE			"/dev/gpio"

/* I/O settings */ 
struct gpio_status {
	int pinNr;			// Number of GPIO pin
	int value;			// when fuction = output: use this value to write. when function = input: this value is actual value of pin
	int function;		// use defines (INPUT, OUTPUT, ALT0, ALT1, ALT2, ALT3, ALT4 or ALT5)
	int pull_up_down;	// use defines (PULL_DOWN_ENABLE, PULL_UP_ENABLE, PULL_OFF)
	int event_detect;	// use defines (NO_DETECT, RISING_EDGE_DETECT, FALLING_EDGE_DETECT, LOW_DETECT, HIGH_DETECT, ASYNC_RISING_EDGE_DETECT or ASYNC_FALLING_EDGE_DETECT
	int irqCount;		// the number of interrupts since last read
};

/* Gpio registers: check datasheet for more information */
struct GpioRegisters
{
	// function select
	long int GPFSEL[6];		// GPIO FUNCTION SELECT:	use this register to set the function to input, output or alternative
	long int Reserved1;
	// output
	long int GPSET[2];		// GPIO SET: 				Use this register to set outputvalue to 1	
	long int Reserved2;
	long int GPCLR[2];		// GPIO CLEAR: 				Use this register to set outputvalue to 0	
	long int Reserved3;
	// input
	long int GPLEV[2];		// GPIO LEVEL:				Use this register to read the actuel value of the pin 
	long int Reserved4;
	// GPIO Pin Detect
	long int GPEDS[2];		// GPIO Pin Event Detect Status			// this register record level and edge event 
	long int Reserved5;
	long int GPREN[2];		// GPIO Pin Rising Edge Detect Enable
	long int Reserved6;
	long int GPFEN[2];		// GPIO Pin Falling Edge Detect Enable
	long int Reserved7;
	long int GPHEN[2];		// GPIO Pin High Detect Enable
	long int Reserved8;
	long int GPLEN[2];		// GPIO Pin Low Detect Enable
	long int Reserved9;
	long int GPAREN[2];		// GPIO Pin Async. Rising Edge Detect
	long int Reserved10;
	long int GPAFEN[2];		// GPIO Pin Async. Falling Edge Detect
	long int Reserved11;
	// Pull-up/down
	long int GPPUD;			// GPIO Pin Pull-up/down Enable
	long int GPPUDCLK[2];	// GPIO Pin Pull-up/down Enable Clock
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

// detect event
#define  	NO_DETECT					0
#define  	RISING_EDGE_DETECT			1
#define  	FALLING_EDGE_DETECT			2

// pull up/down
#define		PULL_DOWN_ENABLE 	0b01		// for use pull down
#define		PULL_UP_ENABLE 		0b10		// for use pull up
#define		PULL_OFF		 	0b00		// for not use pull up/down

#define THERMIOC_MAGIC_GPIO			'G'

#define GPIO_WRITE					_IOW(THERMIOC_MAGIC_GPIO, 0, struct gpio_status)
#define GPIO_READ					_IOR(THERMIOC_MAGIC_GPIO, 1, struct gpio_status)
#define GPIO_SET_CONFIG				_IOW(THERMIOC_MAGIC_GPIO, 2, struct gpio_status)
#define GPIO_GET_IRQCOUNT			_IOR(THERMIOC_MAGIC_GPIO, 3, struct gpio_status)

/* export symbols */
int gpio_write(struct gpio_status* arg);		// write a value to a gpiopin
int gpio_read(struct gpio_status* arg);			// read actual value of gpiopin
int gpio_set_config(struct gpio_status* arg);	// set function and value to a gpiopin
int gpio_get_irqCount(struct gpio_status* arg);	// set function and value to a gpiopin

#endif // GPIO_H
