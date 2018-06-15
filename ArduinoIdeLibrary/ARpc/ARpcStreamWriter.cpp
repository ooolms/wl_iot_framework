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
#include "ARpcConfig.h"
#include <string.h>
#include <stdlib.h>

ARpcStreamWriter::ARpcStreamWriter(ARpcIWriteCallback *wcb)
{
	writeCallback=wcb;
	needArgDelim=false;
	msgFinished=true;
}

bool ARpcStreamWriter::writeMsg(const char *msg,const char **args,unsigned char argsCount)
{
	if(!beginWriteMsg())return false;
	writeArg(msg,strlen(msg));
	for(int i=0;i<argsCount;++i)
		writeArg(args[i],strlen(args[i]));
	endWriteMsg();
	return true;
}

bool ARpcStreamWriter::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	if(!beginWriteMsg())return false;
	writeArg(msg,strlen(msg));
	if(arg1)
		writeArg(arg1,strlen(arg1));
	if(arg2)
		writeArg(arg2,strlen(arg2));
	if(arg3)
		writeArg(arg3,strlen(arg3));
	if(arg4)
		writeArg(arg4,strlen(arg4));
	endWriteMsg();
	return true;
}

bool ARpcStreamWriter::beginWriteMsg()
{
	if(!msgFinished)return false;
	needArgDelim=false;
	writeMsgHeaders();
	return true;
}

void ARpcStreamWriter::writeArg(const char *arg,unsigned long sz)
{
	if(!needArgDelim)
		needArgDelim=true;
	else writeCallback->writeStr("|");
	writeDataEscaped(arg,sz);
}

void ARpcStreamWriter::writeArgNoEscape(const char *arg)
{
	if(!needArgDelim)
		needArgDelim=true;
	else writeCallback->writeStr("|");
	writeCallback->writeStr(arg);
}

void ARpcStreamWriter::writeArgNoEscape(const char *arg,unsigned long sz)
{
	if(!needArgDelim)
		needArgDelim=true;
	else writeCallback->writeStr("|");
	writeCallback->writeData(arg,sz);
}

void ARpcStreamWriter::endWriteMsg()
{
	writeCallback->writeStr("\n");
	needArgDelim=false;
	msgFinished=true;
}

void ARpcStreamWriter::writeMsgHeaders()
{
}

void ARpcStreamWriter::writeDataEscaped(const char *data,unsigned long sz)
{
	char c;
	for(unsigned long i=0;i<sz;++i)
	{
		c=data[i];
		if(c==0)
			writeCallback->writeStr("\\0");
		else if(c=='|')
			writeCallback->writeStr("\\|");
		else if(c=='\n')
			writeCallback->writeStr("\\n");
		else if(c=='\\')
			writeCallback->writeStr("\\\\");
		else writeCallback->writeData(&c,1);
	}
}
