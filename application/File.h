#ifndef FILE_H
#define FILE_H

class File
{
public:
	File( const char* name, int mode )
	: mFd(-1)
	{
		open( name, mode );
	}
	~File()
	{
		close();
	}
	
	bool open( const char* name, int mode )
	{
		close();
		if( name ) {
			mFd = ::open( name, mode );
		}
		return isOpen();
	}
	
	void close()
	{
		if( isOpen() ) {
			::close( mFd );
			mFd = -1;
		}
	}
	
	bool isOpen() const
	{
		return mFd != -1;
	}
	
	template<typename T>
	bool ioctl( int request, T& arg )
	{
		return ::ioctl( mFd, request, reinterpret_cast<long>(&arg) ) != -1;
	}
	
	template <size_t count>
	ssize_t read( char (&buf)[count] )
	{
		return read( buf, count );
	}
	
	ssize_t read(void *buf, size_t count)
	{
		return ::read( mFd, buf, count );
	}
	
	// only define this if std::string is available :)
#if _GLIBCXX_STRING
	bool write(std::string& str)
	{
		return write( str.c_str(), str.length() );
	}
#endif
	
	bool write(const void *buf, size_t count)
	{
		return ::write( mFd, buf, count ) == count;
	}

private:
	int mFd;
};



#endif // FILE_H
