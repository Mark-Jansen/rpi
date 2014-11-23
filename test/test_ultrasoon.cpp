#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ultrasoon_driver/ultrasoon.h>


int read( int fd, struct ultrasoon_data* data )
{
	if( ioctl(fd, ULTRASOON_GET_DISTANCE, data ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	printf( "Distance from sensor type: %d = %d cm\n",data->type, data->distance );
	return 0;
}

int readConfig( int fd, struct ultrasoon_config* cfg )
{
	if( ioctl(fd, ULTRASOON_GET_CONFIG, cfg ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	printf( "Current config:\nTrigger_port: %d\nEcho_port1: %d\nEcho_port2: %d \n", cfg->pinNr_Trigger, cfg->pinNr_echo_1,cfg->pinNr_echo_2);
	return 0;
}

int write( int fd, struct ultrasoon_config* cfg)
{
	printf( "OLD DATA:\nTrigger_port: %d\nEcho_port1: %d\nEcho_port2: %d\n", cfg->pinNr_Trigger, cfg->pinNr_echo_1,cfg->pinNr_echo_2);
	if( ioctl(fd, ULTRASOON_SET_CONFIG, cfg) == -1) {
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	printf( "NEW DATA:\nTrigger_port: %d\nEcho_port1: %d\nEcho_port2: %d\n", cfg->pinNr_Trigger, cfg->pinNr_echo_1,cfg->pinNr_echo_2);
	return 0;
}


int main()
{
	ultrasoon_data data;
	ultrasoon_config cfg;
	
	int fd = open("/dev/ultrasoon", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}
	
	data.distance = 0;
	data.type = 0;
	if( read( fd, &data ) ) {
		return 2;
	}
	
	if( readConfig( fd, &cfg ) ) {
		return 3;
	}
	
	cfg.pinNr_echo_1 = 25;
	if( write( fd, &cfg ) ) {
		return 4;
	}
	
	if( readConfig( fd, &cfg ) ) {
		return 5;
	}
	
	close( fd );
}
