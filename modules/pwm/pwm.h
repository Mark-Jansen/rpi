#ifndef PWM_H
#define PWM_H

struct pwm_settings {
	int  channel;				//channel 0 or 1 (channel 0 is hw_pwm / channel 1 is soft_pwm)  
	int  pin;                   //hardware pwm is on gpio18 (pin 12) and cannot be changed!, software pwm pin can be changed.   
	int  enabled;				//turn pwm on/off
	int  frequency;				//0..20000 (default=1000)
	int  duty_cycle;            //0..100%
};


#define THERMIOC_MAGIC			   'P'

#define PWM_SET_SETTINGS           _IOW(THERMIOC_MAGIC,  0, struct pwm_settings)
#define PWM_GET_SETTINGS           _IOR(THERMIOC_MAGIC,  1, struct pwm_settings)

//channel 1 ioctls
#define PWM_SET_ENABLED_CH0        _IOW(THERMIOC_MAGIC,  2, int )
#define PWM_GET_ENABLED_CH0        _IOR(THERMIOC_MAGIC,  3, int )
#define PWM_SET_DUTY_CYCLE_CH0     _IOW(THERMIOC_MAGIC,  4, int )
#define PWM_GET_DUTY_CYCLE_CH0	   _IOR(THERMIOC_MAGIC,  5, int )
#define PWM_SET_FREQUENCY_CH0      _IOW(THERMIOC_MAGIC,  6, int )
#define PWM_GET_FREQUENCY_CH0	   _IOR(THERMIOC_MAGIC,  7, int )				

//channel 2 ioctls
#define PWM_SET_ENABLED_CH1        _IOW(THERMIOC_MAGIC,  8, int )
#define PWM_GET_ENABLED_CH1        _IOR(THERMIOC_MAGIC,  9, int )
#define PWM_SET_DUTY_CYCLE_CH1     _IOW(THERMIOC_MAGIC, 10, int )
#define PWM_GET_DUTY_CYCLE_CH1	   _IOR(THERMIOC_MAGIC, 11, int )
#define PWM_SET_FREQUENCY_CH1      _IOW(THERMIOC_MAGIC, 12, int )
#define PWM_GET_FREQUENCY_CH1	   _IOR(THERMIOC_MAGIC, 13, int )				


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
