#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <pwm/pwm.h>

struct pwm_settings pwm0;
struct pwm_settings pwm1;


void showMenu( void )
{
printf("\n\nMENU");
printf("\n===========================");
printf("\n[1] Set software pwm pin");
printf("\n[2] Select software pwm");
printf("\n[3] Select hardware pwm");
printf("\n[4] Stop");
printf("\n\nKeuze : ");
}
void showFunctionMenu( void )
{
printf("\n\nMENU");
printf("\n=============");
printf("\n[1] Enabled pwm");
printf("\n[2] Disable pwm");
printf("\n[3] Set frequency");
printf("\n[4] Set dutycycle");
printf("\n[5] Stop");
printf("\n\nKeuze : ");
}

void initialize_structs(void)
{
  /* we have to set the channels so the kernel module can distinguish between the hw and sw pwm */
  pwm0.channel = 0;
  pwm1.channel = 1;
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
    
	initialize_structs();
	read(fd,&pwm0);
	read(fd,&pwm1);
    
    printf("entering program\n");
    usleep(1000000);

	pwm0.enabled = 1;
	pwm1.enabled = 1;
	
	//software pwm simple test
	pwm0.duty_cycle = 0;
	pwm1.duty_cycle = 0;
	write(fd,&pwm0);
	write(fd,&pwm1);
	usleep(5000000);

	pwm0.duty_cycle = 25;
	pwm1.duty_cycle = 25;
	write(fd,&pwm0);
	write(fd,&pwm1);
	usleep(5000000);

	pwm0.duty_cycle = 50;
	pwm1.duty_cycle = 50;
	write(fd,&pwm0);
	write(fd,&pwm1);
	usleep(5000000);

    pwm0.duty_cycle = 75;
	pwm1.duty_cycle = 75;
	write(fd,&pwm0);
	write(fd,&pwm1);
	usleep(5000000);

	pwm0.duty_cycle = 100;
	pwm1.duty_cycle = 100;
	write(fd,&pwm0);
	write(fd,&pwm1);
	usleep(5000000);
     
	pwm0.duty_cycle = 0;
	pwm1.duty_cycle = 0;
    pwm0.enabled = 0;
	pwm1.enabled = 0;
	write(fd,&pwm0);
	write(fd,&pwm1);
	usleep(5000000);
 
	 
	close( fd );
}
