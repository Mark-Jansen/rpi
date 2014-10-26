#ifndef PWM_H
#define PWM_H


struct pwm_config {
    int  channel;				//channel 0 or 1 (channel 0 is hw_pwm / channel 1 is soft_pwm)  
	int  pin;                   //hardware pwm is on gpio18 (pin 12) and cannot be changed!, software pwm pin can be changed. 
};

struct pwm_settings {
    int  enabled;				//turn pwm on/off
	int  frequency;				//0..10000 (default=1000)
	int  duty_cycle;            //0..100%
};

#define THERMIOC_MAGIC			   'P'
#define PWM_SET_CONFIG             _IOW(THERMIOC_MAGIC,  0, struct pwm_config)
#define PWM_GET_CONFIG             _IOR(THERMIOC_MAGIC,  1, struct pwm_config)

#define PWM_SET_SETTINGS           _IOW(THERMIOC_MAGIC,  2, struct pwm_settings)
#define PWM_GET_SETTINGS           _IOR(THERMIOC_MAGIC,  3, struct pwm_settings)

#define PWM_SET_ENABLED            _IOW(THERMIOC_MAGIC,  4, int )
#define PWM_GET_ENABLED            _IOR(THERMIOC_MAGIC,  5, int )
#define PWM_SET_DUTY_CYCLE         _IOW(THERMIOC_MAGIC,  6, int )
#define PWM_GET_DUTY_CYCLE		   _IOR(THERMIOC_MAGIC,  7, int )
#define PWM_SET_FREQUENCY          _IOW(THERMIOC_MAGIC,  8, int )
#define PWM_GET_FREQUENCY		   _IOR(THERMIOC_MAGIC,  9, int )				


//exported symbols:
int pwm_get_config  (struct pwm_config* arg);
int pwm_set_config  (struct pwm_config* arg);
int pwm_get_settings(struct pwm_settings* arg);
int pwm_set_settings(struct pwm_settings* arg);

int pwm_get_enabled   (int channel);
int pwm_set_enabled   (int channel);
int pwm_get_duty_cycle(int channel);
int pwm_set_duty_cycle(int channel, int duty_cycle);
int pwm_get_frequency (int channel);
int pwm_set_frequency (int channel, int frequency);


#endif // PWM_H
