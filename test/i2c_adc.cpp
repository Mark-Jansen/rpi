#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

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
	
	for( int n = 0; n < 20; ++n ) {
		char buf[3] = {0x80};
		if( write( fd, buf, 1 ) != 1 ) {
			perror( "Writing" );
			close( fd );
			return 3;
		}
		if( read( fd, buf, 3) != 3 ) {
			perror( "Reading" );
			close( fd );
			return 4;
		} else {
			printf( "%d, %d, %d\n", buf[0], buf[1], buf[2] );
		}
		sleep(1);
	}	
	
	close( fd );

	return 0;
}
