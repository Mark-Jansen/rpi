#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <adc/adc.h>
#include <string>
#include "../application/File.h"


bool read( File& adc, adc_data& data )
{
	if( !adc.ioctl( ADC_GET_DATA, data ) ) {
		perror( "ioctl get" );
		return false;
	}
	printf( "Value: %d\n", data.value );
	return true;
}

int resolutions[] = {
	ADC_RESOLUTION_12B, ADC_RESOLUTION_14B,
	ADC_RESOLUTION_16B, ADC_RESOLUTION_18B
};

int main()
{
	adc_data data;
	File adc("/dev/adc", O_RDWR);
	if( !adc.isOpen() ) {
		perror( "open" );
		return 1;
	}
	for( size_t n = 0; n < (sizeof(resolutions) / sizeof(resolutions[0])); ++n ) {
		int r = resolutions[n];
		if( adc.ioctl( ADC_GET_MAX_FOR_RES, r ) ) {
			printf( "Res: %x, max: %d\n", resolutions[n], r );
		} else {
			perror( "ADC_GET_MAX_FOR_RES" );
		}
	}
	data.channel = 0;
	if( read( adc, data ) ) {
		data.channel = 1;
		read( adc, data );
	}
	return 0;
}
