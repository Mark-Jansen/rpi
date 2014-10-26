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


extern int debug;

#endif //PWM_INTERNAL_H
