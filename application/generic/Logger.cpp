#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <sys/time.h>

int g_LogLevel = LOGLEVEL_LOG;
int g_LogPrefixTimestamp = 1;
int g_LogPrefixLevel = 1;

const char* LevelToString( int logLevel )
{
	switch( logLevel ) {
		case LOGLEVEL_NONE:	return "NONE";
		case LOGLEVEL_ERR:	return "ERR";
		case LOGLEVEL_WARN:	return "WARN";
		case LOGLEVEL_LOG:	return "LOG ";
		default:			return "????";
	}
}

std::ostream& logger(int level)
{
	if( g_LogPrefixLevel ) {
		std::cout << LevelToString(level);
		if( g_LogPrefixTimestamp ) {
			std::cout << "-";
		}
	}
	if( g_LogPrefixTimestamp ) {
		timeval curTime;
		gettimeofday(&curTime, NULL);
		tm now;
		localtime_r( &curTime.tv_sec, &now );
		// gcc does not support put_time yet :(
		std::cout << std::setfill('0') << std::setw(2) << now.tm_hour << ":" <<
		std::setfill('0') << std::setw(2) << now.tm_min << ":" <<
		std::setfill('0') << std::setw(2) << now.tm_sec << "." <<
		std::setfill('0') << std::setw(4) << (curTime.tv_usec / 1000);
	}
	if( g_LogPrefixLevel || g_LogPrefixTimestamp ) {
		std::cout << "- ";
	}
	return std::cout;
}
