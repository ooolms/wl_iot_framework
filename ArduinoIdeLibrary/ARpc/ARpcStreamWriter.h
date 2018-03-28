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

#ifndef APRCBASE_H
#define APRCBASE_H

typedef void (*ARpcWriteCallback)(void *data,const char *str,unsigned long size);

class ARpcStreamWriter
{
public:
	explicit ARpcStreamWriter(ARpcWriteCallback wcb,void *wcbData);
	void writeMsg(const char *msg,const char *args[],unsigned char argsCount);
	void writeMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeData(const char *byteData,unsigned long sz);
	void writeDataNoEscape(const char *byteData,unsigned long sz);

public:
	static const char *argDelim;
	static const char *msgDelim;

protected:
	ARpcWriteCallback writeCallback;
	void* writeCallbackData;
};

#endif
