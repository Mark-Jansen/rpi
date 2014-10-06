#ifndef ADC_H
#define ADC_H

struct adc_data {
	int channel;
};

#define THERMIOC_MAGIC			'A'
#define ADC_GET_DATA		_IOR(THERMIOC_MAGIC, 0, struct adc_data)

#endif // ADC_H
