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

#define ADC_RESOLUTION_12b				0x0
#define ADC_RESOLUTION_14b				0x4
#define ADC_RESOLUTION_16b				0x8
#define ADC_RESOLUTION_18b				0xc

#define ADC_GAIN_1						0x0
#define ADC_GAIN_2						0x1
#define ADC_GAIN_4						0x2
#define ADC_GAIN_8						0x3

#define THERMIOC_MAGIC		'A'
#define ADC_GET_DATA		_IOWR(THERMIOC_MAGIC, 0, struct adc_data)

#define ADC_GET_CONFIG		_IOR(THERMIOC_MAGIC, 2, struct adc_config)
#define ADC_SET_CONFIG		_IOW(THERMIOC_MAGIC, 3, struct adc_config)

// exported symbols:
int adc_get_data(struct adc_data* data);
int adc_read_device(struct adc_config* cfg, struct adc_data* data);
int adc_get_config(struct adc_config* cfg);
int adc_set_config(struct adc_config* cfg);
//

#endif // ADC_H
