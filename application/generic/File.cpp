#include <string>
#include "File.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

File::File( const char* name, int mode )
: mFd(-1)
{
	open( name, mode );
}
File::~File()
{
	close();
}

bool File::open( const char* name, int mode )
{
	close();
	if( name ) {
		mFd = ::open( name, mode );
	}
	return isOpen();
}

void File::close()
{
	if( isOpen() ) {
		::close( mFd );
		mFd = -1;
	}
}

bool File::isOpen() const
{
	return mFd != -1;
}

ssize_t File::read(void *buf, size_t count)
{
	return ::read( mFd, buf, count );
}

bool File::write(std::string& str)
{
	return write( str.c_str(), str.length() );
}

bool File::write(const void *buf, size_t count)
{
	return ::write( mFd, buf, count ) == count;
}
