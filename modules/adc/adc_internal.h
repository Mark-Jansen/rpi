#ifndef ADC_INTERNAL_H
#define ADC_INTERNAL_H

#define DRV_NAME		"adc"
#define DRV_REV			"r1"
#define ADC_MAJOR		251

#define trace(format, arg...) do { if( debug & 1 ) pr_info( DRV_NAME ": %s: " format "\n", __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( DRV_NAME ": " format "\n", ## arg )
#define warning(format, arg...) pr_warn( DRV_NAME ": " format "\n", ## arg )
#define error(format, arg...) pr_err( DRV_NAME ": " format "\n", ## arg )

extern int debug;

int adc_i2c_init(void);
void adc_i2c_exit(void);

#endif //ADC_INTERNAL_H
