#ifndef GENERIC_LOG_H
#define GENERIC_LOG_H

#include <ostream>

#define LOGLEVEL_NONE	0
#define LOGLEVEL_ERR	1
#define LOGLEVEL_WARN	2
#define LOGLEVEL_LOG	3

extern int g_LogLevel;
extern int g_LogPrefixTimestamp;
extern int g_LogPrefixLevel;

const char* LevelToString( int logLevel );
std::ostream& logger(int level);

#define X_LOGIF(lvl, logX) \
	do { \
		if( lvl <= g_LogLevel ) { \
			logger(lvl) << logX << std::endl; \
		} \
	} while( 0 )

#define ERR(log)		X_LOGIF(LOGLEVEL_ERR, log)
#define WARN(log)		X_LOGIF(LOGLEVEL_WARN, log)
#define LOG(log)		X_LOGIF(LOGLEVEL_LOG, log)

#endif // GENERIC_LOG_H
