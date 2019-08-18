#pragma once
#include <mutex>
#include <stdarg.h>
#include <time.h>

class log_basic
{
public:
	virtual void log(const char* where, const char* what, const char* fmt = 0, ...) = 0;
	virtual void vlog(const char* where, const char* what, const char* fmt, va_list va) = 0;
};

class console_log : public log_basic
{
public:
	void log(const char* where, const char* what, const char* fmt = 0, ...) override
	{
		va_list va;
		va_start(va, fmt);
		vlog(where, what, fmt, va);
		va_end(va);

	}
	void vlog(const char* where, const char* what, const char* fmt, va_list va) override
	{
		char buf[0x200];
		int pos;
		time_t now;
		now = time(0);
#ifdef WIN32
		strcpy(buf, ctime(&now));
#endif // MSVR
#ifndef WIN32
		ctime_r(&now, buf);
#endif // !WIN32

		pos = snprintf(buf + 20, sizeof(buf) - 20, "|%-30s | %-20s | ", where, what);
		pos += 20;
		if (fmt)
		{
			pos += vsnprintf(buf + pos, sizeof(buf) - pos, fmt, va);
		}
		snprintf(buf + pos, sizeof(buf) - pos, "\n");
		{
			std::lock_guard<std::mutex> lock{ _mtx };
			printf(buf);
		}
	}

private:
	std::mutex _mtx;
};

extern console_log _g_default_log;
#define default_log(...) _g_default_log.log(__VA_ARGS__)
#define default_vlog(where, what, fmt, va) _g_default_log.vlog(where, what, fmt, va)