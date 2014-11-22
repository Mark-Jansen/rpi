#ifndef GYRO_H
#define GYRO_H

struct gyro_data {
	int16_t accel_x, accel_y, accel_z;
	int16_t raw_temperature;			// temp = raw_temperature / 340.f + 36.53f
	int16_t gyro_x, gyro_y, gyro_z;
};

#define THERMIOC_MAGIC_GYRO		'G'
#define GYRO_GET_DATA			_IOWR(THERMIOC_MAGIC_GYRO, 0, struct gyro_data)

// exported symbols:
int gyro_get_data(struct gyro_data* data);
//

#endif // GYRO_H
