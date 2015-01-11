#ifndef LED_H
#define LED_H

// gegevens per I/O aansluiting
struct led_status {
	int pinNr;	
	int value;			// ON/OFF	
	int blinkTimer;		// mSec
};

// 
#define		ON				1
#define 	OFF				0

#define 	SEC			1000

#define LED_DEVICE				"/dev/led"

#define THERMIOC_MAGIC_LED		'L'

#define LED_SET_CONFIG	_IOW(THERMIOC_MAGIC_LED, 0, struct led_status)
#define LED_ON			_IOW(THERMIOC_MAGIC_LED, 1, struct led_status)
#define LED_OFF			_IOW(THERMIOC_MAGIC_LED, 2, struct led_status)
#define LED_BLINK		_IOW(THERMIOC_MAGIC_LED, 3, struct led_status)

int led_set_config(struct led_status* arg);
int led_on(struct led_status* arg);		// zou ook in 1 functie kunnen omdat je toch mee geeft wat de geschreven waarde moet zijn. maar ik vind dit overzichtelijker
int led_off(struct led_status* arg);
int led_blink(struct led_status* arg);	

#endif // LED_H
