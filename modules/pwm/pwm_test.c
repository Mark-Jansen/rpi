#include <linux/module.h>
#include <linux/delay.h>
#include "pwm.h"

#define DRV_NAME		"pwm_test"
#define DRV_REV			"r1"
#define PWM_MAJOR	    251


struct pwm_settings pwm1;


static void new_settings(void)
{
	pwm1.channel    = 1;
	pwm1.pin        = 17;
	pwm1.enabled    = 1;
	pwm1.frequency  = 20000;
	pwm1.duty_cycle = 0;
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


static int __init pwm_test_init(void)
{
  new_settings();
  pwm_set_settings(&pwm1);
  simple_test();
  
  return 0;
}

static void __exit pwm_test_exit(void)
{
	
}

module_init(pwm_test_init);
module_exit(pwm_test_exit);

MODULE_AUTHOR("Frank Eggink <freggink@gmail.com>");
MODULE_DESCRIPTION("Pwm test driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);
