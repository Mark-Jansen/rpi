#ifndef LED_H
#define LED_H

// gegevens per I/O aansluiting
struct led_status {
	int channel;
	int value;			
};

// I/O aansluitingen
#define		READY_LED			18
#define		POWER_LED			
#define		LOW_BATTERY_LED		

// 
#define		ON				1
#define 	OFF				0
#define 	OUTPUT			0

#define THERMIOC_MAGIC			'L'

#define LED_ON			_IOW(THERMIOC_MAGIC, 0, struct led_status)
#define LED_OFF			_IOW(THERMIOC_MAGIC, 1, struct led_status)

int led_on(int channel)		// zou ook in 1 functie kunnen omdat je toch mee geeft wat de geschreven waarde moet zijn. maar ik vind dit overzichtelijker
int led_off(int channel)

#endif // LED_H
