/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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
