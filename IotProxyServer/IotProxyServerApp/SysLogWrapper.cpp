#include "SysLogWrapper.h"
#include <sys/syslog.h>

SysLogWrapper SysLogWrapper::instVar=SysLogWrapper();

SysLogWrapper::SysLogWrapper()
{
	openlog("wliotproxyd",0,LOG_USER);
}

SysLogWrapper::~SysLogWrapper()
{
	closelog();
}

void SysLogWrapper::write(int priority,const char *format,va_list args)
{
	syslog(priority,format,args);
}

void SysLogWrapper::write(int priority,const QString &str)
{
	syslog(priority,"%s",str.toUtf8().constData());
}

void SysLogWrapper::setLogLevel(int minPriority)
{
	setlogmask(LOG_UPTO(minPriority));
}
