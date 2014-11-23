#include <linux/module.h>
#include <linux/delay.h>
#include "pwm.h"

#define DRV_NAME		"pwm_test"
#define DRV_REV			"r1"
#define PWM_MAJOR	    251

struct pwm_settings pwm0;
struct pwm_settings pwm1;


static int __init pwm_test_init(void)
{
	int i;

	pwm_get_settings(&pwm0);
	pwm_get_settings(&pwm1);
	msleep(10000);

	pwm_set_duty_cycle(0,100);
	pwm_set_duty_cycle(1,100);

	for(i=0;i<2;i++)
	{
		msleep(1000);
		pwm_set_enabled(0,1);
		pwm_set_enabled(1,1);
		msleep(1000);
		pwm_set_enabled(0,0);
		pwm_set_enabled(1,0);
	}



	//test hw+sw pwm dutycycle
	pwm_set_duty_cycle(0,0);
	pwm_set_duty_cycle(1,0);
	pwm_set_enabled(0,1);
	pwm_set_enabled(1,1);
	msleep(5000);

	pwm_set_frequency(0,25000);
	pwm_set_frequency(1,25000);

	msleep(5000);

	pwm_set_duty_cycle(0,0);
	pwm_set_duty_cycle(1,0);
	msleep(5000);
	pwm_set_duty_cycle(0,25);
	pwm_set_duty_cycle(1,25);
	msleep(5000);
	pwm_set_duty_cycle(0,50);
	pwm_set_duty_cycle(1,50);
	msleep(5000);
	pwm_set_duty_cycle(0,75);
	pwm_set_duty_cycle(1,75);
	msleep(5000);
	pwm_set_duty_cycle(0,100);
	pwm_set_duty_cycle(1,100);
	msleep(5000);



	//test hw+sw pwm frequency	
	pwm_set_duty_cycle(0,50);
	pwm_set_duty_cycle(1,50);

	pwm_set_frequency(0,10000);
	pwm_set_frequency(1,10000);
	msleep(5000);
	pwm_set_frequency(0,5000);
	pwm_set_frequency(1,5000);
	msleep(5000);
	pwm_set_frequency(0,1000);
	pwm_set_frequency(1,1000);
	msleep(5000);
	pwm_set_frequency(0,20000);
	pwm_set_frequency(1,20000);
	msleep(5000);
	pwm_set_frequency(0,25000);
	pwm_set_frequency(1,25000);


	return 0;
}

static void __exit pwm_test_exit(void)
{
	//set pwm settings back to normal
	//set gpio pin back to normal
}

module_init(pwm_test_init);
module_exit(pwm_test_exit);

MODULE_AUTHOR("Frank Eggink <freggink@gmail.com>");
MODULE_DESCRIPTION("Pwm test driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);
