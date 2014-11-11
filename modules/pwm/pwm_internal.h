#ifndef PWM_INTERNAL_H
#define PWM_INTERNAL_H

#define DRV_NAME		"pwm"
#define DRV_REV			"r1"
#define PWM_MAJOR	    252

#define NR_OF_CHANNELS  2
#define CH0				0
#define CH1				1

#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

/* software pwm settings */
#define DEFAULT_GPIO_OUTPUT          17
#define MICRO_SEC               1000000
#define NANO_SEC       (MICRO_SEC*1000)
#define DEFAULT_FREQ              20000
#define DEFAULT_DUTY_CYCLE            0
#define MAX_DUTY_CYCLE              100
#define MAX_FREQUENCY			 100000


/* hardware pwm settings */
#define GP_BASE      (BCM2708_PERI_BASE + 0x200000) //todo use gpio from stefan!!
#define PWM_BASE     (BCM2708_PERI_BASE + 0x20C000)
#define CLOCK_BASE   (BCM2708_PERI_BASE + 0x101000)
#define PWMMODE 1
#define MSMODE  2

//todo use gpio from stefan!!
struct GpioRegisters
{
	uint32_t GPFSEL[6];
	uint32_t Reserved1;
	uint32_t GPSET[2];
	uint32_t Reserved2;
	uint32_t GPCLR[2];
};


struct PwmRegisters
{
	uint32_t CTL;
	uint32_t STA;
	uint32_t DMAC;
	uint32_t Reserved1;
	uint32_t RNG1;
	uint32_t DAT1;
	uint32_t FIF1;
	uint32_t Reserved2;
	uint32_t RNG2;
	uint32_t DAT2;
};

struct ClockRegisters
{
	uint32_t Unknown[40];
	uint32_t PWMCTL;
	uint32_t CLKDIV;
};



extern int debug;

#endif //PWM_INTERNAL_H
