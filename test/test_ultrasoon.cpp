#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "../modules/ultrasoon/ultrasoon.h" 


int read( int fd, struct ultrasoon_data* data )
{
	char frontSensor[6] = {'F', 'r', 'o', 'n', 't', '\0'};
	char backSensor[5] = {'B', 'a', 'c', 'k', '\0'};
	
	if( ioctl(fd, ULTRASOON_GET_DISTANCE, data ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	if(data->type <= 0)
		printf( "Distance from %s sensor: %d cm\n",frontSensor, data->distance );
	else
		printf( "Distance from %s sensor: %d cm\n",backSensor, data->distance );
	
	return 0;
}

int readConfig( int fd, struct ultrasoon_config* cfg )
{
	if( ioctl(fd, ULTRASOON_GET_CONFIG, cfg ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	printf( "Current config:\nTrigger_port: %d\nEcho_port1: %d\nEcho_port2: %d \n", cfg->pinNr_Trigger_1, cfg->pinNr_echo_1,cfg->pinNr_echo_2);
	return 0;
}

int writeConfig( int fd, struct ultrasoon_config* cfg)
{
	printf( "DATA to SET the config:\nTrigger_port: %d\nEcho_port1: %d\nEcho_port2: %d\n", cfg->pinNr_Trigger_1, cfg->pinNr_echo_1,cfg->pinNr_echo_2);
	if( ioctl(fd, ULTRASOON_SET_CONFIG, cfg) == -1) {
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	
	printf( "GELUKT\n");
	return 0;
}


int main(int arc, char **argv)
{
	ultrasoon_data data;
	ultrasoon_config cfg;
	
	data.distance = 0;
	data.type = 0;
		
	int fd = open("/dev/ultrasoon", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}
	
	if( readConfig( fd, &cfg ) ) {
		return 2;
	}
	
	cfg.pinNr_Trigger_1 = 28;
	cfg.pinNr_echo_1 = 30;
	
	if( writeConfig( fd, &cfg ) ) {
		return 3;
	}
	
	if( read( fd, &data ) ) {
		return 4;
	}
	
	cfg.pinNr_Trigger_1 = 29;
	cfg.pinNr_echo_1 = 31;
	
	printf( "SET new config for 2e sensor \n");
	if( writeConfig( fd, &cfg ) ) {
		return 5;
	}
	
	if( read( fd, &data ) ) {
		return 6;
	}
	
	close( fd );
}
