#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		
#include <linux/hrtimer.h> 
#include <linux/delay.h> 
#include <asm/io.h>
#include "pwm.h"
#include "pwm_internal.h"
#include "../gpio/gpio.h"

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

static DEFINE_SPINLOCK(g_Lock);

static struct pwm_settings g_Settings[NR_OF_CHANNELS];

/* hardware pwm */
struct PwmRegisters    *s_PwmRegisters;
struct ClockRegisters  *s_ClockRegisters;
/* end of hardware pwm */

/* gpio settings */
struct gpio_status hw_pwm;
struct gpio_status sw_pwm;
/* end of gpio settings */

int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");

/* start of software pwm code*/
struct hrtimer tm1, tm2; //define timer1 and timer2
static ktime_t t1; 

/* rising edge of software pwm */
enum hrtimer_restart cb1(struct hrtimer *t) {
	ktime_t now;
	int ovr;
	now = hrtimer_cb_get_time(t); //get current time 
	ovr = hrtimer_forward(t, now, t1); //set timer1 to be called again in t1(frequency) from now (creating rising edge of pwm)

	if (g_Settings[SW_PWM_CH].duty_cycle && g_Settings[SW_PWM_CH].enabled) {
		sw_pwm.value = 1;
		gpio_write(&sw_pwm);
		if (g_Settings[SW_PWM_CH].duty_cycle <= MAX_DUTY_CYCLE) {
			unsigned long t_ns = ((MICRO_SEC * 10 * g_Settings[SW_PWM_CH].duty_cycle) / (g_Settings[SW_PWM_CH].frequency)); //calculate time when to generate falling edge
			ktime_t t2 = ktime_set( 0, t_ns ); //convert time from nanoseconds into ktime_t
			hrtimer_start(&tm2, t2, HRTIMER_MODE_REL); //start timer2 which expires after t2 (dutycycle) (let timer2 create falling edge)
		}
	}
	else {
		sw_pwm.value = 0;
		gpio_write(&sw_pwm);
	}
	return HRTIMER_RESTART;
}

/* falling edge of software pwm */
enum hrtimer_restart cb2(struct hrtimer *t) {

	sw_pwm.value = 0;
	gpio_write(&sw_pwm);	
	return HRTIMER_NORESTART;
}
/* end of software pwm code */


void configHardwarePwm(void)
{
	const int scaleFactor = 100000;
	int period;
	int countDuration;
	int divisor;
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags ); 

	//stop clock and waiting for busy flag doesn't work, so kill clock
	s_ClockRegisters->PWMCTL = 0x5A000000 | (1 << 5);
	udelay(10);  

	//wait until busy flag is set 
	while (s_ClockRegisters->PWMCTL & 0x00000080){}   

	//calculate divisor value for PWM1 clock...base frequency is 19.2MHz, use scaleFactor to get more accurate result because we can't use floating point in a kernel module
	period = (1 * scaleFactor) /  g_Settings[HW_PWM_CH].frequency;
	countDuration = period * scaleFactor / (COUNTS);
	divisor = 19200000 / ( scaleFactor / countDuration);
	divisor = divisor / scaleFactor;
	trace("calculated divisor= %d",divisor);

	//set divisor
	s_ClockRegisters->CLKDIV = 0x5A000000 | divisor << 12;

	//source=osc and enable clock
	s_ClockRegisters->PWMCTL = 0x5A000011;

	//disable PWM & start from a clean slate
	s_PwmRegisters->CTL = 0;

	// needs some time until the PWM module gets disabled, without the delay the PWM module crashes
	udelay(10); 

	s_PwmRegisters->RNG1 = COUNTS; //pwm resolution (counts == 256 -> 8bit pwm)

	if(g_Settings[HW_PWM_CH].enabled == TRUE) 
	{
		// start PWM1 in
		if(mode == PWMMODE) //PWM mode 
			s_PwmRegisters->CTL |= (1 << 0); 
		else // M/S Mode
			s_PwmRegisters->CTL |= ((1 << 7) | (1 << 0));
	}
	else
	{
		s_PwmRegisters->CTL = 0;  /* turn pwm off */
	}
	spin_unlock_irqrestore( &g_Lock, flags );
}


void update_duty_cycle(void)
{       
	unsigned long flags;
	int dutyCycle = g_Settings[HW_PWM_CH].duty_cycle;
	int newCount = (((COUNTS * 1000) / 100) * dutyCycle) / 1000;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	trace("newCount = %d",newCount);
	s_PwmRegisters->DAT1=newCount; //new dutyCycle
	spin_unlock_irqrestore( &g_Lock, flags );
}
/* end of hardware pwm code */


/* start of exported symbols */
int pwm_get_settings(struct pwm_settings* arg)
{
	unsigned long flags;
	trace("channel = %d",arg->channel);
	spin_lock_irqsave( &g_Lock, flags );
	arg->channel    = g_Settings[arg->channel].channel;
	arg->pin        = g_Settings[arg->channel].pin;
	arg->frequency  = g_Settings[arg->channel].frequency;
	arg->duty_cycle = g_Settings[arg->channel].duty_cycle;
	arg->enabled    = g_Settings[arg->channel].enabled;
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_get_settings);

int pwm_set_settings(struct pwm_settings* arg)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );

	//changes to software pwm
	if(arg->channel == SW_PWM_CH)
	{
		if (g_Settings[SW_PWM_CH].pin != arg->pin) //is the software pwm pin has changed
		{
			//turn old gpio pin off
			sw_pwm.pinNr = g_Settings[SW_PWM_CH].pin;
			sw_pwm.value = 0;
			gpio_write(&sw_pwm);

			//turn new gpio pin on
			sw_pwm.pinNr = arg->pin;
			sw_pwm.function = OUTPUT;
			gpio_set_config(&sw_pwm);
		}
	}
	g_Settings[arg->channel].channel    = arg->channel;
	g_Settings[arg->channel].pin        = arg->pin;
	g_Settings[arg->channel].frequency  = arg->frequency;
	g_Settings[arg->channel].duty_cycle = arg->duty_cycle;
	g_Settings[arg->channel].enabled    = arg->enabled;
	if(arg->channel == HW_PWM_CH) 
	{ 
		update_duty_cycle(); 
		configHardwarePwm(); // update all pwm settings
	}
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_set_settings);


int pwm_set_enabled(int channel, int enabled)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	g_Settings[channel].enabled = enabled;
	if(channel == HW_PWM_CH)
	{
		//hw pwm
		if(enabled == FALSE) 
		{
			s_PwmRegisters->CTL = 0;  /* turn pwm off */
		}
		else
		{
			// start PWM1 in
			if(mode == PWMMODE) { s_PwmRegisters->CTL |=  (1 << 0); } 
			else                { s_PwmRegisters->CTL |= ((1 << 7) | (1 << 0)); } //MSMODE
		} 
	}
	if(channel == SW_PWM_CH)
	{
		//softpwm
		//software pwm is already enabled or disabled above
	}
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_set_enabled);


int pwm_get_enabled(int channel)
{
	unsigned long flags;
	int enabled;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	enabled = g_Settings[channel].enabled;
	spin_unlock_irqrestore( &g_Lock, flags );
	return enabled;
}
EXPORT_SYMBOL(pwm_get_enabled);


int pwm_set_duty_cycle(int channel, int duty_cycle)
{
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	g_Settings[channel].duty_cycle = duty_cycle;
	if(channel == HW_PWM_CH)
	{
		//hw pwm
		update_duty_cycle();
	}
	if(channel == SW_PWM_CH)
	{
		//softpwm
	}
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_set_duty_cycle);


int pwm_get_duty_cycle(int channel)
{
	unsigned long flags;
	int dutyCycle;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	dutyCycle = g_Settings[channel].duty_cycle;
	spin_unlock_irqrestore( &g_Lock, flags );
	return dutyCycle;
}
EXPORT_SYMBOL(pwm_get_duty_cycle);


int pwm_set_frequency (int channel, int frequency)
{   
	unsigned long flags;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	g_Settings[channel].frequency = frequency;
	if(channel == HW_PWM_CH)
	{
		//hw pwm
		configHardwarePwm(); //TODO
	}
	if(channel == SW_PWM_CH)
	{
		//softpwm
	}
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_set_frequency);

int pwm_get_frequency(int channel)
{
	unsigned long flags;
	int frequency;
	trace("");
	spin_lock_irqsave( &g_Lock, flags );
	frequency = g_Settings[channel].frequency;
	spin_unlock_irqrestore( &g_Lock, flags );
	return frequency;
}
EXPORT_SYMBOL(pwm_get_frequency);
/* end of exported symbols */


// file operations
static long pwm_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	long ret = -EFAULT;
	struct pwm_settings settings;
	trace("");
	switch( command ) {
	case PWM_SET_SETTINGS:
		if( copy_from_user( &settings, (void*)arg, sizeof(struct pwm_settings) ) )
			return -EFAULT;
		ret = pwm_set_settings( &settings );
		break;
	case PWM_GET_SETTINGS:
		/* first copy current setting to determine channel!! */
		if( copy_from_user( &settings, (void*)arg, sizeof(struct pwm_settings) ) )
			return -EFAULT;
		if( !pwm_get_settings( &settings ) )
			ret = 0;
		else
			return -ENXIO;
		if( copy_to_user( (void*)arg, &settings, sizeof(struct pwm_settings) ) )
			return -EFAULT;
		break;
	default:
		return ENOTTY;
	}
	return ret;
}


static const struct file_operations pwm_fops = {
	.owner				= THIS_MODULE,
	.unlocked_ioctl		= pwm_ioctl,
};


//sysfs
static ssize_t show_pwmx(struct device *dev, struct device_attribute *attr, char *buf, int chan)
{
	struct pwm_settings s;
	trace("show pwmx channel is: %d\n",chan );
	s.channel = chan;
	if( pwm_get_settings( &s ) ) 
		return snprintf( buf, PAGE_SIZE, "Failed to read the data\n" );
	return snprintf(buf, PAGE_SIZE, "Channel: %d, pin: %d, enabled: %d, frequency: %d, duty_cycle: %d\n",
		s.channel,                                                                                
		s.pin,
		s.enabled,
		s.frequency,
		s.duty_cycle);
}

static ssize_t show_pwm0(struct device *dev, struct device_attribute *attr, char *buf)
{
	return show_pwmx( dev, attr, buf, 0 );						
}

static ssize_t show_pwm1(struct device *dev, struct device_attribute *attr, char *buf)
{
	return show_pwmx( dev, attr, buf, 1 );					
}


static DEVICE_ATTR( pwm0, S_IWUSR | S_IRUGO, show_pwm0, NULL );
static DEVICE_ATTR( pwm1, S_IWUSR | S_IRUGO, show_pwm1, NULL );


static void software_pwm_init(void)
{
	unsigned long t_ns;
	g_Settings[SW_PWM_CH].channel    = SW_PWM_CH;
	g_Settings[SW_PWM_CH].pin        = DEFAULT_GPIO_OUTPUT;
	g_Settings[SW_PWM_CH].frequency  = DEFAULT_FREQ;
	g_Settings[SW_PWM_CH].duty_cycle = 0;
	g_Settings[SW_PWM_CH].enabled    = FALSE; 

	t_ns = (NANO_SEC)/DEFAULT_FREQ; 
	hrtimer_init(&tm1, CLOCK_MONOTONIC, HRTIMER_MODE_REL); //initialize timer tm1
	hrtimer_init(&tm2, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	t1 = ktime_set( 0, t_ns ); //set ktime_t variable from a seconds/nanoseconds value
	tm1.function = &cb1; //set callback function for timer1
	tm2.function = &cb2; //set callback function for timer2

	//initialize sw pwm gpio pin
	sw_pwm.pinNr = g_Settings[SW_PWM_CH].pin;
	//sw_pwm.value = 0;
	sw_pwm.function = OUTPUT; 
	gpio_set_config(&sw_pwm);

	hrtimer_start(&tm1, t1, HRTIMER_MODE_REL); //start timer tm1, with t1 as expire time, in relative mode
}

static void software_pwm_exit(void)
{
	if (hrtimer_active(&tm1) || hrtimer_is_queued(&tm1)) 
	{
		hrtimer_cancel(&tm1);
	}
	if (hrtimer_active(&tm2) || hrtimer_is_queued(&tm2)) 
	{
		hrtimer_cancel(&tm2);
	}
	//free sw pwm gpio pin
	sw_pwm.pinNr = g_Settings[SW_PWM_CH].pin;
	sw_pwm.value = 0;
	gpio_write(&sw_pwm);
}


static void hardware_pwm_init(void)
{ 
	g_Settings[HW_PWM_CH].channel    = HW_PWM_CH;
	g_Settings[HW_PWM_CH].pin        = DEFAULT_PWM_PIN;
	g_Settings[HW_PWM_CH].frequency  = DEFAULT_FREQ;
	g_Settings[HW_PWM_CH].duty_cycle = 0;
	g_Settings[HW_PWM_CH].enabled    = FALSE;

	s_PwmRegisters   = (struct PwmRegisters*)ioremap(PWM_BASE, sizeof(struct PwmRegisters));     
	s_ClockRegisters = (struct ClockRegisters*)ioremap(CLOCK_BASE, sizeof(struct ClockRegisters)); 

	//configure hw pwm pin (alt5)
	hw_pwm.pinNr = g_Settings[HW_PWM_CH].pin;
	hw_pwm.function = ALT5;
	gpio_set_config(&hw_pwm);

	configHardwarePwm();

	s_PwmRegisters->DAT1 = 0; //dutyCycle is set to zero
}

static void hardware_pwm_exit(void)
{
	//Configure the pin as input 
	hw_pwm.function = INPUT;
	gpio_set_config(&hw_pwm);
}



static int __init pwm_init(void)
{
	int ret = -1;
	trace("");    

	g_Major = register_chrdev( PWM_MAJOR, DRV_NAME, &pwm_fops );
	if( g_Major < 0 ) {
		error( "could not register device: %d", g_Major );
		goto out;
	} if( g_Major == 0 ) {
		g_Major = PWM_MAJOR;
	}

	g_Class = class_create( THIS_MODULE, DRV_NAME );
	if( IS_ERR(g_Class) ) {
		error( "could not register class %s", DRV_NAME );
		ret = PTR_ERR( g_Class );
		goto out_chrdev;
	}

	g_Device = device_create( g_Class, NULL, MKDEV(g_Major, 0), NULL, DRV_NAME );
	if( IS_ERR(g_Class) ) {
		error( "could not create %s", DRV_NAME );
		ret = PTR_ERR( g_Device );
		goto out_device;
	}

	ret  = device_create_file( g_Device, &dev_attr_pwm0 );
	ret |= device_create_file( g_Device, &dev_attr_pwm1 );
	info( DRV_REV " loaded, major: %d", g_Major );

	software_pwm_init(); 
	hardware_pwm_init(); 

	goto out;

out_device:
	class_unregister( g_Class );
	class_destroy( g_Class );
out_chrdev:
	unregister_chrdev(g_Major, DRV_NAME);

out:
	return ret;
}

static void __exit pwm_exit(void)
{
	trace("");
	device_remove_file( g_Device, &dev_attr_pwm0 );
	device_remove_file( g_Device, &dev_attr_pwm1 );
	device_destroy( g_Class, MKDEV(g_Major, 0) );
	class_unregister( g_Class );
	class_destroy( g_Class );
	unregister_chrdev( g_Major, DRV_NAME );

	software_pwm_exit(); 
	hardware_pwm_exit(); 
	info("unloaded.");
}

module_init(pwm_init);
module_exit(pwm_exit);

MODULE_AUTHOR("Frank Eggink <freggink@gmail.com>");
MODULE_DESCRIPTION("Pwm driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_REV);
