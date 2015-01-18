#ifndef ULTRASOON_H
#define ULTRASOON_H

struct ultrasoon_data {
	int distance;		//Distance in CM.
	int type;			//0 == Sensor_1 , 1== Sensor_2
};

struct ultrasoon_config{
	int pinNr_Trigger_1;	//default 0
	int pinNr_Trigger_2;	//default 0
	int pinNr_echo_1;		//default 0
	int pinNr_echo_2;		//default 0 // only used if 2 sensors on 1 object
};

#define FRONT_SENSOR	0
#define BACK_SENSOR		1
#define	HIGH			1
#define LOW				0

#define ULTRASOON_DEVICE		"/dev/ultrasoon"

#define THERMIOC_MAGIC_ULTRA	'E'
#define ULTRASOON_GET_DISTANCE	_IOR(THERMIOC_MAGIC_ULTRA, 0, struct ultrasoon_data) 	

#define ULTRASOON_GET_CONFIG	_IOR(THERMIOC_MAGIC_ULTRA, 2, struct ultrasoon_config)	
#define ULTRASOON_SET_CONFIG	_IOW(THERMIOC_MAGIC_ULTRA, 3, struct ultrasoon_config)	

		
int ultrasoon_get_distance(struct ultrasoon_data* cfg);
int ultrasoon_get_config(struct ultrasoon_config* cfg);
int ultrasoon_set_config(struct ultrasoon_config* cfg);
		

		
#endif // ULTRASOON_H
