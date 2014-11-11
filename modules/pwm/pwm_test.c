#include <linux/module.h>
#include <linux/delay.h>
#include "pwm.h"

#define DRV_NAME		"pwm_test"
#define DRV_REV			"r1"
#define PWM_MAJOR	    251

struct pwm_settings pwm0;
struct pwm_settings pwm1;


static void new_settings(void)
{
	pwm1.channel    = 1;
	pwm1.pin        = 17;
	pwm1.enabled    = 1;
	pwm1.frequency  = 20000;
	pwm1.duty_cycle = 0;
	
	pwm0.duty_cycle = 0;
}


static void simple_test(void)
{
	volatile int i;
	for(i=0; i <= 100; i+=20)
	{
		pwm1.duty_cycle = i;
		pwm_set_settings(&pwm1);
		msleep(1000);
	}

	for(i=100; i >= 0; i-=20)
	{
		pwm1.duty_cycle = i;
		pwm_set_settings(&pwm1);
		msleep(1000);
	}

}

static void simple_hw_test(void)
{
	volatile int i;
	for(i=0; i <= 100; i+=20)
	{
		pwm0.duty_cycle = i;
		pwm_set_settings(&pwm0);
		msleep(1000);
	}

	for(i=100; i >= 0; i-=20)
	{
		pwm0.duty_cycle = i;
		pwm_set_settings(&pwm0);
		msleep(1000);
	}
}

static int __init pwm_test_init(void)
{
	int i;
    new_settings();
	pwm_set_settings(&pwm1);
    for(i=0;i<2;i++)
    {
	  simple_test();
	  simple_hw_test();
    }
    //test hw duty_cycle
	msleep(1000);
	pwm_set_duty_cycle(0,100);
	msleep(2000);
	pwm_set_duty_cycle(0,50);
	msleep(2000);
	pwm_set_duty_cycle(0,0);
	msleep(2000);
	//test sw duty cycle
	pwm_set_duty_cycle(1,100);
	msleep(2000);
	pwm_set_duty_cycle(1,50);
	msleep(2000);
	pwm_set_duty_cycle(1,0);
	msleep(2000);
	
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
