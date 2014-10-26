#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <pwm/pwm.h>

struct pwm_settings pwm0;
struct pwm_settings pwm1;


void initialize_pwm_settings()
{
    pwm0.channel    = 0;
	pwm0.pin        = 18;
	pwm0.enabled    = 0;
	pwm0.frequency  = 0;
	pwm0.duty_cycle = 0;
	
	pwm1.channel    = 1;
	pwm1.pin        = 17;
	pwm1.enabled    = 0;
	pwm1.frequency  = 0;
	pwm1.duty_cycle = 0;
}

void new_settings()
{
    pwm0.channel    = 0;
	pwm0.pin        = 18;
	pwm0.enabled    = 1;
	pwm0.frequency  = 1000;
	pwm0.duty_cycle = 80;
	
	pwm1.channel    = 1;
	pwm1.pin        = 17;
	pwm1.enabled    = 1;
	pwm1.frequency  = 2000;
	pwm1.duty_cycle = 50;

}

void show_settings(struct pwm_settings *s)
{
    printf("Channel: %d, pin: %d, enabled: %d, frequency: %d, duty_cycle: %d\n",s->channel,
                                                                                s->pin,
																				s->enabled,
																				s->frequency,
																				s->duty_cycle);
} 


int read( int fd, struct pwm_settings* settings )
{
	if( ioctl(fd, PWM_GET_SETTINGS , settings ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	show_settings(settings);
	return 0;
}

int write( int fd, struct pwm_settings* settings)
{
	if( ioctl(fd, PWM_SET_SETTINGS , settings ) == -1) {
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	return 0;
}


int main()
{
    
	int fd = open("/dev/pwm", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}
	
	initialize_pwm_settings();
	
	read(fd,&pwm0);
	read(fd,&pwm1);
	
	new_settings();
	
	write(fd,&pwm0);
	write(fd,&pwm1);
	
	read(fd,&pwm0);
	read(fd,&pwm1);

	read(fd,&pwm0);
	read(fd,&pwm1);
	
	read(fd,&pwm0);
	read(fd,&pwm1);
	
	

/*
	settings.enabled = ioctl(fd,PWM_GET_ENABLED, settings.enabled);
	printf("pwm enabled is %d\n",settings.enabled); 
	printf("changing enabled to true\n");
	ioctl(fd,PWM_SET_ENABLED, 1);
	settings.enabled = ioctl(fd,PWM_GET_ENABLED, settings.enabled);
	printf("pwm enabled is %d\n", settings.enabled);

	settings.frequency = ioctl(fd,PWM_GET_FREQUENCY, settings.frequency);
	printf("pwm frequency is %d\n",settings.frequency); 
	printf("changing frequencty to 20000\n");
	ioctl(fd,PWM_SET_FREQUENCY, 20000);
	settings.frequency = ioctl(fd,PWM_GET_FREQUENCY, settings.frequency);
	printf("pwm frequency is %d\n", settings.frequency);
	
	settings.duty_cycle = ioctl(fd,PWM_GET_DUTY_CYCLE, settings.duty_cycle);
	printf("pwm duty_cycle is %d\n",settings.duty_cycle); 
	printf("changing duty_cycle to 80\n");
	ioctl(fd,PWM_SET_DUTY_CYCLE, 80);
	settings.duty_cycle = ioctl(fd,PWM_GET_DUTY_CYCLE, settings.duty_cycle);
	printf("pwm duty_cycle is %d\n", settings.duty_cycle);
	*/	
		
	close( fd );
}
