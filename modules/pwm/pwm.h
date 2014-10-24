#ifndef PWM_H
#define PWM_H

struct pwm_status {
	int status;
};

struct pwm_config {
    bool enabled;
    int  period;
	int  frequency;
	int  duty_cycle;
};


#define THERMIOC_MAGIC			   'P'
#define PWM_SET_STATUS		       _IOW(THERMIOC_MAGIC,  0, struct pwm_status)
#define PWM_GET_STATUS		       _IOR(THERMIOC_MAGIC,  1, struct pwm_status)

#define PWM_SET_CONFIG             _IOW(THERMIOC_MAGIC,  2, struct pwm_config)
#define PWM_GET_CONFIG             _IOR(THERMIOC_MAGIC,  3, struct pwm_config)


#define PWM_SET_ENABLED            _IOW(THERMIOC_MAGIC,  4, bool)
#define PWM_GET_ENABLED            _IOR(THERMIOC_MAGIC,  5, bool)
#define PWM_SET_PERIOD             _IOW(THERMIOC_MAGIC,  6,  int)
#define PWM_GET_PERIOD             _IOR(THERMIOC_MAGIC,  7,  int)
#define PWM_SET_FREQUENCY          _IOW(THERMIOC_MAGIC,  8,  int)
#define PWM_GET_FREQUENCY          _IOR(THERMIOC_MAGIC,  9,  int)
#define PWM_SET_DUTY_CYCLE         _IOW(THERMIOC_MAGIC, 10,  int)
#define PWM_GET_DUTY_CYCLE         _IOR(THERMIOC_MAGIC, 11,  int)
#endif // PWM_H
