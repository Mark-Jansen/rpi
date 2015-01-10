#ifndef ADC_INTERNAL_H
#define ADC_INTERNAL_H

#define DRV_NAME		"adc"

int adc_i2c_init(void);
void adc_i2c_exit(void);

// normalized resolution.
unsigned long adc_conversionTime( int resolution );
int adc_decode( int* result, char buf[4] );

#endif //ADC_INTERNAL_H
