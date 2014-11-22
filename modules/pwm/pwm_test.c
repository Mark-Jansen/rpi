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
	pwm1.enabled    = 0; //was 1, changed it for testing enabled/disabled
	pwm1.frequency  = 25000;
	pwm1.duty_cycle = 0;

	//we can only set duty_cycle for hardware pwm
	//the rest of the settings are fixed for now
	//hw pwm channel = 0
	//hw pwm pin     = 18
	//hw pwm freq    = 25000 Hz
	//hw pwm is enabled by default and dutycyle is set to zero by default!!
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
	//new_settings();
/*
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
*/


    pwm_get_settings(&pwm0);
	pwm_get_settings(&pwm1);
	msleep(10000);
    //test hw+sw pwm enabled/disabled
	//pwm_set_settings(&pwm0);
	//pwm_set_settings(&pwm1);
/*	pwm_set_duty_cycle(0,100);
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
	
    pwm_set_duty_cycle(0,25);
	pwm_set_duty_cycle(1,25);
	msleep(2000);
	pwm_set_duty_cycle(1,50);
    pwm_set_duty_cycle(1,50);
	msleep(2000);
	pwm_set_duty_cycle(0,75);
	pwm_set_duty_cycle(1,75);
	msleep(2000);
    pwm_set_duty_cycle(0,100);
	pwm_set_duty_cycle(1,100);
	msleep(2000);
*/



	
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
