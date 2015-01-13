#ifndef PWM_H
#define PWM_H

/* always initialize channel to 0 or 1, before changing any other setting */
struct pwm_settings {
	int  channel;				//channel 0 or 1 (channel 0 is hw_pwm / channel 1 is soft_pwm)  
	int  pin;                   //hardware pwm is on gpio18 (pin 12) and cannot be changed!, software pwm pin can be changed.   
	int  enabled;				//turn pwm on/off
	int  frequency;				//0..25000Hz (default=25kHz)
	int  duty_cycle;            //0..100%
};


#define PWM_DEVICE			"/dev/pwm"

#define PWM_THERMIOC_MAGIC			'P'

#define PWM_SET_SETTINGS           _IOW(PWM_THERMIOC_MAGIC,  0, struct pwm_settings)
#define PWM_GET_SETTINGS           _IOR(PWM_THERMIOC_MAGIC,  1, struct pwm_settings)


//exported symbols:
int pwm_get_settings(struct pwm_settings* arg);
int pwm_set_settings(struct pwm_settings* arg);

int pwm_get_enabled   (int channel);
int pwm_set_enabled   (int channel, int enabled);
int pwm_get_duty_cycle(int channel);
int pwm_set_duty_cycle(int channel, int duty_cycle);
int pwm_get_frequency (int channel);
int pwm_set_frequency (int channel, int frequency);


#endif // PWM_H
