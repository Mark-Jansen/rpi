#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>		//copy_[from/to]_user
#include <linux/gpio.h>         //TODO use gpio from stefan!!
#include <linux/hrtimer.h> 
#include <linux/delay.h> 
#include <asm/io.h>

#include "pwm.h"
#include "pwm_internal.h"

static int g_Major = 0;
static struct class* g_Class = NULL;
static struct device* g_Device = NULL;

static DEFINE_SPINLOCK(g_Lock);

static struct pwm_settings g_Settings[NR_OF_CHANNELS];

/* hardware pwm */
struct GpioRegisters   *s_pGpioRegisters; //TODO use gpio from stefan!!
struct PwmRegisters    *s_PwmRegisters;
struct ClockRegisters  *s_ClockRegisters;

//relocate and cleanup this code
static const int pwmPin = 18;
double frequency        = 1000.0; // PWM frequency
double dutyCycle        = 50.0;   // PWM duty Cycle (%)
unsigned int counts     = 256;    // PWM resolution
unsigned int divisor    = 75;     // divisor value
int mode = PWMMODE;               // PWM mode
/* end of hardware pwm */


int debug = 0;
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "set debug flags, 1 = trace");


/* start of software pwm code*/
struct hrtimer tm1, tm2;
static ktime_t t1;

enum hrtimer_restart cb1(struct hrtimer *t) {
	ktime_t now;
	int ovr;
	now = hrtimer_cb_get_time(t);
	ovr = hrtimer_forward(t, now, t1);

	if (g_Settings[CH1].duty_cycle && g_Settings[CH1].enabled) {
		gpio_set_value(g_Settings[CH1].pin, 1);
		if (g_Settings[CH1].duty_cycle <= MAX_DUTY_CYCLE) {
			unsigned long t_ns = ((MICRO_SEC * 10 * g_Settings[CH1].duty_cycle) / (g_Settings[CH1].frequency));
			ktime_t t2 = ktime_set( 0, t_ns );
			hrtimer_start(&tm2, t2, HRTIMER_MODE_REL);
		}
	}
	else {
		gpio_set_value(g_Settings[CH1].pin, 0);
	}
	return HRTIMER_RESTART;
}


enum hrtimer_restart cb2(struct hrtimer *t) {

	gpio_set_value(g_Settings[CH1].pin, 0);
	return HRTIMER_NORESTART;
}
/* end of software pwm code */



/* start of hardware pwm code */
static void SetGPIOFunction(int GPIO, int functionCode)
{
	int registerIndex = GPIO / 10;
	int bit = (GPIO % 10) * 3;

	unsigned oldValue = s_pGpioRegisters->GPFSEL[registerIndex];
	unsigned mask = 0b111 << bit;
	printk("Changing function of GPIO%d from %x to %x\n", GPIO, (oldValue >> bit) & 0b111, functionCode);
	s_pGpioRegisters->GPFSEL[registerIndex] = (oldValue & ~mask) | ((functionCode << bit) & mask);
}


void configHardwarePwm(void)
{
	//	double period;
	//	double countDuration;

	// stop clock and waiting for busy flag doesn't work, so kill clock
	s_ClockRegisters->PWMCTL = 0x5A000000 | (1 << 5);
	udelay(10);  

	// wait until busy flag is set 
	while (s_ClockRegisters->PWMCTL & 0x00000080){}   

	//set divisor
	s_ClockRegisters->CLKDIV = 0x5A000000 | divisor << 12;

	// source=osc and enable clock
	s_ClockRegisters->PWMCTL = 0x5A000011;

	// disable PWM & start from a clean slate
	s_PwmRegisters->CTL = 0;

	// needs some time until the PWM module gets disabled, without the delay the PWM module crashs
	udelay(10); 

	//s_PwmRegisters->RNG1 = counts;
	//s_PwmRegisters->DAT1 = 0; //dutycycle

	// start PWM1 in
	if(mode == PWMMODE) //PWM mode 
		s_PwmRegisters->CTL |= (1 << 0); 
	else // M/S Mode
		s_PwmRegisters->CTL |= ((1 << 7) | (1 << 0));
}


void update_hw_pwm_settings(void)
{
	//s_PwmRegisters->RNG1=counts;
	int dutyCycle = g_Settings[CH0].duty_cycle;
	int newCount = (counts / 100) * dutyCycle;
	s_PwmRegisters->DAT1=newCount;
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
	g_Settings[arg->channel].channel    = arg->channel;
	g_Settings[arg->channel].pin        = arg->pin;
	g_Settings[arg->channel].frequency  = arg->frequency;
	g_Settings[arg->channel].duty_cycle = arg->duty_cycle;
	g_Settings[arg->channel].enabled    = arg->enabled;
	if(arg->channel == 0) { update_hw_pwm_settings(); } //todo
	spin_unlock_irqrestore( &g_Lock, flags );
	return 0;
}
EXPORT_SYMBOL(pwm_set_settings);


int pwm_set_enabled(int channel, int enabled)
{
	g_Settings[channel].enabled = enabled;
	if(channel==0)
	{
		//hw pwm
		update_hw_pwm_settings();
	}
	if(channel==1)
	{
		//softpwm
	}
	return 0;
	//update todo
}
EXPORT_SYMBOL(pwm_set_enabled);


int pwm_set_duty_cycle(int channel, int duty_cycle)
{
	g_Settings[channel].duty_cycle = duty_cycle;
	if(channel==0)
	{
		//hw pwm
		update_hw_pwm_settings();
	}
	if(channel==1)
	{
		//softpwm
	}
	return 0;
}
EXPORT_SYMBOL(pwm_set_duty_cycle);

int pwm_set_frequency (int channel, int frequency)
{
	g_Settings[channel].frequency = frequency;
	if(channel==0)
	{
		//hw pwm
		update_hw_pwm_settings();
	}
	if(channel==1)
	{
		//softpwm
	}
	return 0;
}
EXPORT_SYMBOL(pwm_set_frequency);
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
		/*......................................................*/	
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
/*
static ssize_t duty_cycle_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t len) {
	unsigned int dc = 0;
	if (!kstrtouint(buf, 10, &dc)) {
		dc = dc > MAX_DUTY_CYCLE ? MAX_DUTY_CYCLE : dc;
		g_Settings[CH1].duty_cycle = dc;
		trace("duty set to %d\n",dc);
		trace("global duty set to %d\n",g_Settings[CH1].duty_cycle);
	}
	return len;
}
static ssize_t duty_cycle_show(struct device *dev,struct device_attribute *attr, char *buf) {

	return sprintf(buf, "%d", g_Settings[CH1].duty_cycle);
}

static ssize_t frequency_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t len) {
	unsigned int fr = 0;
	if (!kstrtouint(buf, 10, &fr)) {
		fr = fr > MAX_FREQUENCY ? MAX_FREQUENCY : fr;
		g_Settings[CH1].frequency = fr;
		trace("frequency set to %d\n",fr);
		trace("global frequency set to %d\n",g_Settings[CH1].frequency);
	}
	return len;
}
static ssize_t frequency_show(struct device *dev,struct device_attribute *attr, char *buf) {

	return sprintf(buf, "%d", g_Settings[CH1].frequency);
}
*/

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
//static DEVICE_ATTR( duty_cycle, S_IWUSR | S_IRUGO, duty_cycle_show, duty_cycle_store );
//static DEVICE_ATTR( frequency, S_IWUSR | S_IRUGO, frequency_show, frequency_store );


static void software_pwm_init(void)
{
	unsigned long t_ns;
	g_Settings[CH1].pin = DEFAULT_GPIO_OUTPUT; //TODO
	g_Settings[CH1].frequency = DEFAULT_FREQ;
	g_Settings[CH1].duty_cycle = 0;

	t_ns = (NANO_SEC)/DEFAULT_FREQ;
	hrtimer_init(&tm1, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_init(&tm2, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	t1 = ktime_set( 0, t_ns );
	tm1.function = &cb1;
	tm2.function = &cb2;
	gpio_request(g_Settings[CH1].pin, "soft_pwm_gpio");
	gpio_direction_output(g_Settings[CH1].pin, 1);
	hrtimer_start(&tm1, t1, HRTIMER_MODE_REL);
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
	gpio_free(g_Settings[CH1].pin);
}


static void hardware_pwm_init(void)
{ 
	s_pGpioRegisters = (struct GpioRegisters  *) __io_address(GP_BASE); //todo
	s_PwmRegisters   = (uint32_t *)ioremap(PWM_BASE, sizeof(struct PwmRegisters));
	s_ClockRegisters = (uint32_t *)ioremap(CLOCK_BASE, sizeof(struct ClockRegisters));

	SetGPIOFunction(pwmPin, 0b000); //Configure the pin as input (default)
	SetGPIOFunction(pwmPin, 0b010);	//Configure the pin as pwm pin

	configHardwarePwm();

	s_PwmRegisters->RNG1=counts;
	s_PwmRegisters->DAT1=0; //dutyCycle is set to zero

}

static void hardware_pwm_exit(void)
{
	SetGPIOFunction(pwmPin, 0);	//Configure the pin as input 
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
	//ret |= device_create_file( g_Device, &dev_attr_duty_cycle );
	//ret |= device_create_file( g_Device, &dev_attr_frequency );
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
	//device_remove_file( g_Device, &dev_attr_duty_cycle );
	//device_remove_file( g_Device, &dev_attr_frequency );
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
