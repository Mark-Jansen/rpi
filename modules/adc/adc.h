#ifndef ADC_H
#define ADC_H

struct adc_data {
	int channel;			// channel to read from (0 or 1)
	int value;				// result
};

struct adc_config {
	int resolution;
	int gain;
};

#define ADC_RESOLUTION_12B				0x0
#define ADC_RESOLUTION_14B				0x4
#define ADC_RESOLUTION_16B				0x8
#define ADC_RESOLUTION_18B				0xc

#define ADC_GAIN_1						0x0
#define ADC_GAIN_2						0x1
#define ADC_GAIN_4						0x2
#define ADC_GAIN_8						0x3

#define THERMIOC_MAGIC_ADC		'A'
#define ADC_GET_DATA		_IOWR(THERMIOC_MAGIC_ADC, 0, struct adc_data)

#define ADC_GET_CONFIG		_IOR(THERMIOC_MAGIC_ADC, 2, struct adc_config)
#define ADC_SET_CONFIG		_IOW(THERMIOC_MAGIC_ADC, 3, struct adc_config)

#define ADC_GET_MAX_FOR_RES _IOWR(THERMIOC_MAGIC_ADC, 4, int)	// the int should be used as pointer!!

// exported symbols:
int adc_get_data(struct adc_data* data);
int adc_read_device(struct adc_config* cfg, struct adc_data* data);
int adc_get_config(struct adc_config* cfg);
int adc_set_config(struct adc_config* cfg);
int adc_max_for_res(int resolution);
//


// constants
#define ADC_VREF			2.048

#define ADC_RES_12B_MIN		-2048
#define ADC_RES_12B_MAX		2047

#define ADC_RES_14B_MIN		-8192
#define ADC_RES_14B_MAX		8191

#define ADC_RES_16B_MIN		-32768
#define ADC_RES_16B_MAX		32767

#define ADC_RES_18B_MIN		-131072
#define ADC_RES_18B_MAX		131071


#endif // ADC_H
