#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include <pwm/pwm.h>

using namespace std;

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
	printf("\n[1] Enable pwm");
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
	bool close_menu = false;
	char choice = 0;
	char function = 0;
	int pinnr;
	int frequency, dutycycle;
	int fd = open("/dev/pwm", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}

	initialize_structs();
	read(fd,&pwm0);
	read(fd,&pwm1);

	while(!close_menu)
	{ 
		showMenu();
		cin.get(choice);
		cout << "Choice = " << choice << endl;
		cin.ignore();
		switch(choice)
		{
		case '1':
			cout << "Pinnumber to config" << endl;
			cin >> pinnr;
			cin.ignore();
			pwm1.pin = pinnr;
			if( ioctl(fd, PWM_SET_SETTINGS, &pwm1) == -1)
			{   
				perror( "ioctl PWM_SET_SETTINGS: failed" );
				close( fd );
				return -1;
			}
			break;
		case '2':
			showFunctionMenu();
			cin >> function;
			cin.ignore();
			switch(function)
			{
			case '1':
				pwm1.enabled = 1;
				break;
			case '2':
				pwm1.enabled = 0;
				break;
			case '3':
				cout << "Enter Frequency: "<< endl;
				cin >> frequency;
				cin.ignore();
				pwm1.frequency = frequency;

				break;
			case '4':
				cout << "Enter Dutycycle: "<< endl;
				cin >> dutycycle;
				cin.ignore();
				pwm1.duty_cycle = dutycycle;
			}
			if( ioctl(fd, PWM_SET_SETTINGS, &pwm1) == -1)
			{   
				perror( "ioctl PWM_SET_SETTINGS: failed" );
				close( fd );
				return -1;
			}
			break;
		case '3':
			showFunctionMenu();
			cin >> function;
			cin.ignore();
			switch(function)
			{
			case '1':
				pwm0.enabled = 1;
				break;
			case '2':
				pwm0.enabled = 0;
				break;
			case '3':
				cout << "Enter Frequency: "<< endl;
				cin >> frequency;
				cin.ignore();
				pwm0.frequency = frequency;

				break;
			case '4':
				cout << "Enter Dutycycle: "<< endl;
				cin >> dutycycle;
				cin.ignore();
				pwm0.duty_cycle = dutycycle;
			}
			if( ioctl(fd, PWM_SET_SETTINGS, &pwm0) == -1)
			{   
				perror( "ioctl PWM_SET_SETTINGS: failed" );
				close( fd );
				return -1;
			}
			break;

		case '4':
			close_menu = true;
			close ( fd );
			break;
			if( ioctl(fd, PWM_SET_SETTINGS, &pwm1) == -1)
			{   
				perror( "ioctl PWM_SET_SETTINGS: failed" );
				close( fd );
				return -1;
				break;
			}
		}
	}
	close( fd );
}
