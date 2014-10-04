#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <example/example.h>


int read( int fd, struct example_status* status )
{
	if( ioctl(fd, EXAMPLE_GET_STATUS, status ) == -1) {
		perror( "ioctl get" );
		close( fd );
		return -1;
	}
	printf( "Status: %d\n", status->status );
	return 0;
}

int write( int fd, struct example_status* status )
{
	if( ioctl(fd, EXAMPLE_SET_STATUS, status ) == -1) {
		perror( "ioctl set" );
		close( fd );
		return -1;
	}
	return 0;
}

int main()
{
	example_status status;
	int fd = open("/dev/example", O_RDWR);
	if (fd == -1) {
		perror( "open" );
		return 1;
	}
	status.status = 0;
	if( read( fd, &status ) ) {
		return 2;
	}
	status.status++;
	if( write( fd, &status ) ) {
		return 3;
	}
	if( read( fd, &status ) ) {
		return 4;
	}
	close( fd );
}
