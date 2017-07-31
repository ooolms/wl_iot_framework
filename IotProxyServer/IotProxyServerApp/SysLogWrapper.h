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
