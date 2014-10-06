#ifndef ADC_H
#define ADC_H

struct adc_data {
	int channel;			// channel to read from
	int value;				// result
};

struct adc_config {
	int mode;
};

#define ADC_MODE_8BIT					0
#define ADC_MODE_10BIT					1
#define ADC_MODE_COMPLEMENTARY_FILTER	2

#define THERMIOC_MAGIC		'A'
#define ADC_GET_DATA		_IOWR(THERMIOC_MAGIC, 0, struct adc_data)

#define ADC_GET_CONFIG		_IOR(THERMIOC_MAGIC, 2, struct adc_config)
#define ADC_SET_CONFIG		_IOW(THERMIOC_MAGIC, 3, struct adc_config)

//
int adc_get_data(struct adc_data* arg);

int adc_get_config(struct adc_config* arg);
int adc_set_config(struct adc_config* arg);
//

#endif // ADC_H
