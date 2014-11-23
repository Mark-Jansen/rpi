#ifndef FILE_H
#define FILE_H

#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>

class File
{
public:
	File( const char* name, int mode );
	~File();

	bool open( const char* name, int mode );
	void close();
	bool isOpen() const;

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
	ssize_t read(void *buf, size_t count);

	// only define this if std::string is available :)
#if _GLIBCXX_STRING
	bool write(std::string& str);
#endif

	bool write(const void *buf, size_t count);

private:
	int mFd;
};



#endif // FILE_H
