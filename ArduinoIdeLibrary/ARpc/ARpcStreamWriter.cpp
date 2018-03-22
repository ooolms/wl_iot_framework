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

#include "ARpcStreamWriter.h"
#include "ARpcUuid.h"
#include <string.h>
#include <stdlib.h>

ARpcStreamWriter::ARpcStreamWriter(ARpcWriteCallback wcb,void *wcbData)
{
	writeCallback=wcb;
	writeCallbackData=wcbData;
}

void ARpcStreamWriter::writeMsg(const char *msg,const char *args[],unsigned char argsCount)
{
	writeData(msg,strlen(msg));
	for(int i=0;i<argsCount;++i)
	{
		writeCallback(writeCallbackData,"|",1);
		writeData(args[i],strlen(args[i]));
	}
	writeCallback(writeCallbackData,"\n",1);
}

void ARpcStreamWriter::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	writeData(msg,strlen(msg));
	if(arg1)
	{
		writeCallback(writeCallbackData,"|",1);
		writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		writeCallback(writeCallbackData,"|",1);
		writeData(arg2,strlen(arg2));
	}
	if(arg3)
	{
		writeCallback(writeCallbackData,"|",1);
		writeData(arg3,strlen(arg3));
	}
	if(arg4)
	{
		writeCallback(writeCallbackData,"|",1);
		writeData(arg4,strlen(arg4));
	}
	writeCallback(writeCallbackData,"\n",1);
}

void ARpcStreamWriter::writeData(const char *byteData,unsigned long sz)
{
	char c;
	for(unsigned long i=0;i<sz;++i)
	{
		c=byteData[i];
		if(c==0)
			writeCallback(writeCallbackData,"\\0",2);
		else if(c=='|')
			writeCallback(writeCallbackData,"\\|",2);
		else if(c=='\n')
			writeCallback(writeCallbackData,"\\n",2);
		else if(c=='\\')
			writeCallback(writeCallbackData,"\\\\",2);
		else writeCallback(writeCallbackData,&c,1);
	}
}

void ARpcStreamWriter::writeDataNoEscape(const char *byteData,unsigned long sz)
{
	writeCallback(writeCallbackData,byteData,sz);
}
