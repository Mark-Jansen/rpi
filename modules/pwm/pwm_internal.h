#ifndef PWM_INTERNAL_H
#define PWM_INTERNAL_H

#define DRV_NAME		"pwm"
#define DRV_REV			"r1"
#define PWM_MAJOR	    252

#define NR_OF_CHANNELS  2


#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

/* software pwm settings */
#define DEFAULT_GPIO_OUTPUT 17
#define MICRO_SEC 1000000
#define NANO_SEC (MICRO_SEC*1000)
#define DEFAULT_FREQ 20000
#define DEFAULT_DUTY_CYCLE 0


extern int debug;

#endif //PWM_INTERNAL_H
