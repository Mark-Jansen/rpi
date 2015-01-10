#ifndef GYRO_INTERNAL_H
#define GYRO_INTERNAL_H

#define DRV_NAME		"gyro"

int gyro_i2c_init(void);
void gyro_i2c_exit(void);

int gyro_read_device(struct gyro_data* data);

#endif // GYRO_INTERNAL_H
