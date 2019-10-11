#include <cstdlib>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

int main()
{
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log.properties"));
	Logger logger = Logger::getRoot();
	// trace
	
	LOG4CPLUS_TRACE(logger, "trace and get the fingerprint: " << "random integer: " << 123);
	// debug
	LOG4CPLUS_DEBUG(logger, "this is debug log: " << "random integer: " << 456);
	// info
	LOG4CPLUS_INFO(logger, "the information centry...." << "[ 1 + 1 = " << 1 + 1 << "]");
	// warn
	LOG4CPLUS_WARN(logger, "Writing warning messages to log....");
	// error
	LOG4CPLUS_ERROR(logger, "ooooooh, there is an error....");
	//fatal
	LOG4CPLUS_FATAL(logger, "oh, my god! the fatal error occur!!!!!!!!!");
	//
	Logger loggerTest = log4cplus::Logger::getInstance(L"test");
	LOG4CPLUS_TRACE(loggerTest, "anther logger,trace");
	LOG4CPLUS_ERROR(loggerTest, "anther logger,error ");
	return 0;
}