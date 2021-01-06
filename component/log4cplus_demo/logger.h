#ifndef LL_LOGGER_H
#define LL_LOGGER_H
#include <cstdlib>
#include <stdarg.h>
#include <stdio.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>
extern log4cplus::Logger gLogger;

namespace Log{
using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

inline bool initLogger(){
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log.properties"));
	log4cplus::Logger tmp = Logger::getRoot();
	gLogger = tmp;
	return true;
}

enum{
	INFO,
	TRACE,
	DEBUG,
	WARN,
	ERROR,
	FATAL
};

inline std::string vlog(const char* fmt, va_list va)
{
    char buf[0x200] = {0};
    int pos = 0;
    if (fmt)
    {
        pos += vsnprintf(buf + pos, sizeof(buf) - pos, fmt, va);
    }
	printf("%s", buf);
	return std::string(buf);
}

inline void log(int level, const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    std::string res = vlog(fmt, va);
	printf("%s", res.c_str());
    va_end(va);
	switch(level){
	case INFO:
		LOG4CPLUS_INFO(gLogger, res.c_str());
		break;
	case TRACE:
		LOG4CPLUS_TRACE(gLogger, res.c_str());
		break;
	case DEBUG:
		LOG4CPLUS_DEBUG(gLogger, res.c_str());
		break;
	case WARN:
		LOG4CPLUS_WARN(gLogger, res.c_str());
		break;
	case ERROR:
		LOG4CPLUS_ERROR(gLogger, res.c_str());
		break;
	case FATAL:
		LOG4CPLUS_FATAL(gLogger, res.c_str());
		break;
	default:
		break;
	};
}

}

#define LOG_INFO(...)  Log::log(Log::INFO, __VA_ARGS__)
#define LOG_TRACE(...) Log::log(Log::TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) Log::log(Log::DEBUG, __VA_ARGS__)
#define LOG_WARN(...)  Log::log(Log::WARN, __VA_ARGS__)
#define LOG_ERROR(...) Log::log(Log::ERROR, __VA_ARGS__)
#define LOG_FATAL(...) Log::log(Log::FATAL, __VA_ARGS__)


#endif
