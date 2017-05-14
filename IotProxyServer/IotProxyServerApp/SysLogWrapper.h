#ifndef SYSLOGWRAPPER_H
#define SYSLOGWRAPPER_H

#include <stdarg.h>
#include <QString>

//priorities from sys/syslog.h
class SysLogWrapper
{
private:
	SysLogWrapper();
	~SysLogWrapper();
	SysLogWrapper(const SysLogWrapper &);
	SysLogWrapper& operator=(const SysLogWrapper &);

public:
	static void write(int priority,const char *format,va_list args);
	static void write(int priority,const QString &str);
	static void setLogLevel(int minPriority);

private:
	static SysLogWrapper instVar;
};

#endif // SYSLOGWRAPPER_H
