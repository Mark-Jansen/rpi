#ifndef GPIO_H
#define GPIO_H

// gegevens per I/O aansluiting
struct gpio_status {
	int channel;	// Nummer van GPIO pin
	int value;		// voor input de actuele waarde van de GPIOpin. Voor output de waarde die je naar de GPIOpin wilt sturen
	int function;	// input = 1, output = 0, zie defines
};

// registers die goed gezet moeten worden aan de hand van de datasheet
// deze bevat alle gegevens van alle I/O aansluitingen.
struct GpioRegisters
{
	// function select
	uint32_t GPFSEL[6];		// GPIO SELECT:	Deze kun je gebruiken om een gekozen pin input of output te maken	// check datasheet
	uint32_t Reserved1;
	// output
	uint32_t GPSET[2];		// GPIO SET: 	Deze kun je gebruiken om de gekozen pin 1 te maken					// check datasheet
	uint32_t Reserved2;
	uint32_t GPCLR[2];		// GPIO CLEAR: 	Deze kun je gebruiken om de gekozen pin 0 te maken					// check datasheet
	uint32_t Reserved3;
	// input
	uint32_t GPLEV[2];		// GPIO LEVEL:	Deze geeft de actuele waarden van de GPIO pin
};

// I/O aansluitingen
#define		READY_LED			18
#define		POWER_LED			
#define		LOW_BATTERY_LED	
#define 	BUTTON	
// motor
#define		PWM_A				
#define		PWM_B				
#define		MOTORDRIVER_AIN1		
#define		MOTORDRIVER_AIN2	
#define		MOTORDRIVER_BIN1	
#define		MOTORDRIVER_BIN2	
#define 	MOTORDRIVER_STANDBY
// encoder
#define		ENCODER_A_A
#define		ENCODER_A_B
#define		ENCODER_B_A
#define		ENCODER_B_B		
// i2c
#define		I2C_SDA
#define		I2C_SCL
// ultrasoon sensor	
#define		ULTRASOON_A_TRIG
#define		ULTRASOON_A_ECHO
#define		ULTRASOON_B_TRIG
#define		ULTRASOON_B_ECHO

// 
#define		OUTPUT				0b001		// check datasheet
#define 	INPUT				0b000		// check datasheet

#define THERMIOC_MAGIC			'G'

#define GPIO_WRITE			_IOW(THERMIOC_MAGIC, 0, struct gpio_status)
#define GPIO_READ			_IOR(THERMIOC_MAGIC, 1, struct gpio_status)
#define GPIO_SET_CONFIG		_IOR(THERMIOC_MAGIC, 2, struct gpio_status)

int gpio_write(struct gpio_status* arg);
int gpio_read(struct gpio_status* arg);
int gpio_set_config(struct gpio_status* arg);

#endif // GPIO_H
