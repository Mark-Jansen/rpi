#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdint.h>

namespace Resolution
{
	enum TYPE {
		b12 = 0x0,
		b14 = 0x4,
		b16 = 0x8,
		b18 = 0xc
	};
}

// see (2208c.pdf page 5)
unsigned long conversionTime( Resolution::TYPE res )
{
	switch( res ) {
		case Resolution::b12: return 4167; 		// 240 SPS
		case Resolution::b14: return 16667; 	// 60 SPS
		case Resolution::b16: return 66667; 	// 15 SPS
		case Resolution::b18: return 266667; 	// 3.75 SPS
	}
	return 0;
}

static const uint8_t kNotReady = 0x80;
static const uint8_t kNewConversion = 0x80;
static const uint8_t kContinuous = 0x10;

#define SIGNEXTEND( r, b, e )	do {\
	if( (r & b) != 0 )\
		r |= e;\
	} while( 0 )

static int decode( long& result, char buf[4] )
{
	int status, dataSize;
	if( (buf[3] & 0x0c) == 0x0c ) {
		status = buf[3];
		dataSize = 3;
	} else {
		status = buf[2];
		dataSize = 2;
	}
	if( status & kNotReady )
		return 3;

	result = 0;
	for( int i = 0; i < dataSize; ++i ) {
		result <<= 8;
		result |= (long)buf[i];
	}

	switch( Resolution::TYPE(status&0xc) ) {
	case Resolution::b12:
		SIGNEXTEND( result, 0x800, 0xFFFFF000 );
		break;
	case Resolution::b14:
		SIGNEXTEND( result, 0x2000, 0xFFFFC000 );
		break;
	case Resolution::b16:
		SIGNEXTEND( result, 0x8000, 0xFFFF0000 );
		break;
	case Resolution::b18:
		SIGNEXTEND( result, 0x20000, 0xFFFC0000 );
		break;
	default:
		return 4;
	}
	return 0;
}

int readADC( long& result, int fd, int chan = 0, Resolution::TYPE resolution = Resolution::b14, uint8_t gain = 0 )
{
	char buf[4] = { kNewConversion | ((chan&1)<<5) | (resolution & 0xc) | (gain & 0x3) };
	if( write( fd, buf, 1 ) != 1 )
		return 1;
	usleep( conversionTime( resolution ) );
	if( read( fd, buf, 4) != 4 )
		return 2;
	return decode( result, buf );
}


int main()
{
	int fd = open("/dev/i2c-1", O_RDWR);
	if (fd < 0) {
		perror( "Opening bus" );
		return 1;
	}

	if( ioctl( fd, I2C_SLAVE, 0x68 ) < 0 ) {
		perror( "Selecting address" );
		close( fd );
		return 2;
	}
	
	int res = 0;
	for( int n = 0; n < 50; ++n ) {
		long res;
		int r;
		if( !(r=readADC( res, fd, 0 )) ) {
			printf( "ch#0: %d\t", res );
		} else {
			printf( "ch#0: ERR %d\t", r );
		}
		if( !(r=readADC( res, fd, 1 )) ) {
			printf( "ch#1: %d", res );
		} else {
			printf( "ch#1: ERR %d", r );
		}
		printf( "\n" );
		usleep( 500000 );
	}
	
	close( fd );

	return 0;
}
