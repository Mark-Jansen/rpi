#ifndef ENCODER_H
#define ENCODER_H

struct 	encoder_data {
	int encoder1_pinnr;
	int encoder2_pinnr;
	int rotation_speed;
	int direction;      // 0 is left , 1 is right
};

#define THERMIOC_MAGIC_ENCODER 			'E'
#define ENCODER_SET_CONFIG				_IOW(THERMIOC_MAGIC_ENCODER, 1, struct encoder_data)
#define ENCODER_SPEED					_IOR(THERMIOC_MAGIC_ENCODER, 2, struct encoder_data)
#define ENCODER_DIRECTION				_IOR(THERMIOC_MAGIC_ENCODER, 3, struct encoder_data)


// =================================================================================
// int encoder_data_set_config(struct encoder_data* arg)
// Pre : 
// Post: 
// =================================================================================
int encoder_data_set_config(struct encoder_data* arg);

// =================================================================================
// int get_pulse_count(struct encoder_data* arg);
// Pre : 
// Post: get pulse count 
// =================================================================================
int get_rotation_speed(struct encoder_data* arg);

// =================================================================================
// int get_direction(struct encoder_data* arg);
// Pre : 
// Post: get the direction 0 = left 1 = right
// =================================================================================
int get_direction(struct encoder_data* arg);

#endif // ENCODER_H