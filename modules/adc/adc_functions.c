#include <linux/module.h>
#include <linux/errno.h>
#include "adc_internal.h"

// see (2208c.pdf page 5)
unsigned long adc_conversionTime( int resolution )
{
	switch( resolution ) {
		case 0x0: return 4167; 		// 240 SPS
		case 0x4: return 16667; 	// 60 SPS
		case 0x8: return 66667; 	// 15 SPS
		case 0xc: return 266667; 	// 3.75 SPS
	}
	warning("invalid resolution");
	return 0;
}

#define SIGNEXTEND( r, b, e )	do {\
	if( (r & b) != 0 )\
		r |= e;\
	} while( 0 )

static const unsigned char kNotReady = 0x80;

int adc_decode( int* result, char buf[4] )
{
	int res, status, dataSize, i;
	if( (buf[3] & 0x0c) == 0x0c ) {
		status = buf[3];
		dataSize = 3;
	} else {
		status = buf[2];
		dataSize = 2;
	}
	if( status & kNotReady ) {
		return -EAGAIN;
	}

	res = 0;
	for( i = 0; i < dataSize; ++i ) {
		res <<= 8;
		res |= (int)buf[i];
	}

	switch( status & 0xc ) {
	case 0:
		SIGNEXTEND( res, 0x800, 0xFFFFF000 );
		break;
	case 4:
		SIGNEXTEND( res, 0x2000, 0xFFFFC000 );
		break;
	case 8:
		SIGNEXTEND( res, 0x8000, 0xFFFF0000 );
		break;
	case 0xc:
		SIGNEXTEND( res, 0x20000, 0xFFFC0000 );
		break;
	default:
		return -EINVAL;
	}
	*result = res;
	return 0;
}

