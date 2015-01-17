#include "Controller.h"
#include <generic/Logger.h>
#include <getopt.h>
#include <iostream>

void print_usage(const char* name)
{
	std::cout << "Usage: " << name << " [tvnewlh]" << std::endl;
	std::cout << "optional arguments:" << std::endl;
	std::cout << "  -t, --disable-log-timestamp     Disable logging timestamps" << std::endl;
	std::cout << "  -v, --disable-log-level         Disable level prefix on logging" << std::endl;
	std::cout << "  -n, --loglevel-none             Disable all logging" << std::endl;
	std::cout << "  -e, --loglevel-err              Only show errors in the output" << std::endl;
	std::cout << "  -w, --loglevel-warn             Only show warnings and errors" << std::endl;
	std::cout << "  -l, --loglevel-log              Show all logging (default)" << std::endl;
	std::cout << "  -h, --help                      Show this message and exit" << std::endl;
}

int main(int argc, char* argv[])
{
	static struct option long_options[] = {
		{"disable-log-timestamp",	no_argument, NULL, 't' },
		{"disable-log-level",		no_argument, NULL, 'v' },
		{"loglevel-none",			no_argument, NULL, 'n' },
		{"loglevel-err",			no_argument, NULL, 'e' },
		{"loglevel-warn",			no_argument, NULL, 'w' },
		{"loglevel-log",			no_argument, NULL, 'l' },
		{"help",					no_argument, NULL, 'h' },
		{0,							0,			 0, 0 }
	};
	int long_index = 0, opt = 0;
	while( (opt = getopt_long(argc, argv, "tvnewlh", long_options, &long_index) ) != -1 ) {
		switch( opt ) {
		case 't':
			g_LogPrefixTimestamp = 0;
			break;
		case 'v':
			g_LogPrefixLevel = 0;
			break;
		case 'n':
			g_LogLevel = LOGLEVEL_NONE;
			break;
		case 'e':
			g_LogLevel = LOGLEVEL_ERR;
			break;
		case 'w':
			g_LogLevel = LOGLEVEL_WARN;
			break;
		case 'l':
			g_LogLevel = LOGLEVEL_LOG;
			break;
		case 'h':
		default:
			print_usage(argv[0]);
			return -1;
		}
	}
	Controller controller;
	return controller.run();
	return 0;
}
