#ifndef ULTRASOON_H
#define ULTRASOON_H

struct ultrasoon_data {
	int distance;		//Distance in CM.
	int type;			//0 == Front sensor , 1== Back sensor
};

struct ultrasoon_config{
	int pinNr_Trigger;
	int pinNr_echo_1;
	int pinNr_echo_2;
};

#define FRONT_SENSOR			0;
#define BACK_SENSOR				1;

#define THERMIOC_MAGIC_ULTRA	'E'
#define ULTRASOON_GET_DISTANCE	_IOR(THERMIOC_MAGIC_ULTRA, 0, struct ultrasoon_data) 	

#define ULTRASOON_GET_CONFIG	_IOR(THERMIOC_MAGIC_ULTRA, 2, struct ultrasoon_config)	
#define ULTRASOON_SET_CONFIG	_IOW(THERMIOC_MAGIC_ULTRA, 3, struct ultrasoon_config)	

		
int ultrasoon_get_distance(struct ultrasoon_data* cfg);
int ultrasoon_get_config(struct ultrasoon_config* cfg);
int ultrasoon_set_config(struct ultrasoon_config* cfg);
		

		
#endif // ULTRASOON_H
