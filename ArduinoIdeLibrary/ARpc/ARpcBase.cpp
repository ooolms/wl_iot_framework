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

#include "ARpcBase.h"
#include <string.h>
#include <stdlib.h>

ARpcBase::ARpcBase(unsigned long bSize,ARpcWriteCallback wcb)
{
	writeCallback=wcb;
	bufSize=bSize;
	currentArgIndex=0;
	buffer=(char*)malloc(bufSize+1);
	memset(buffer,0,bufSize+1);
	bufIndex=0;
}

ARpcBase::~ARpcBase()
{
	free(buffer);
}

void ARpcBase::putChar(char c)
{
	if(c==0)
	{
		reset();
		return;
	}
	if(state==ESCAPE)
		parseCharInEscapeState(c);
	else if(state==ESCAPE_HEX1)
	{
		hexChars[0]=c;
		state=ESCAPE_HEX2;
	}
	else if(state==ESCAPE_HEX2)
	{
		hexChars[1]=c;
		state=NORMAL;
		buffer[bufIndex]=(charFromHexChar(hexChars[0])<<4)+charFromHexChar(hexChars[1]);
		++bufIndex;
	}
	else //NORMAL
		parseCharInNormalState(c);
	if(bufIndex==bufSize)//переполнение буфера, эпик фейл
		reset();
}

void ARpcBase::reset()
{
	bufIndex=0;
	buffer[0]=0;
	currentArgIndex=0;
}

void ARpcBase::parseCharInNormalState(char c)
{
	if(c=='\\')
		state=ESCAPE;
	else if(c=='|')
	{
		buffer[bufIndex]=0;
		++bufIndex;
		++currentArgIndex;
		if(currentArgIndex!=0)
			args[currentArgIndex-1]=&buffer[bufIndex];
	}
	else if(c=='\n')
	{
		processMessage(buffer,args,currentArgIndex);
		reset();
	}
	else
	{
		buffer[bufIndex]=c;
		++bufIndex;
	}
}

void ARpcBase::parseCharInEscapeState(char c)
{
	if(c=='x')
	{
		state=ESCAPE_HEX1;
		return;
	}

	if(c=='n')
		buffer[bufIndex]='\n';
	else if(c=='0')
		buffer[bufIndex]=0;
	else
		buffer[bufIndex]=c;
	++bufIndex;
	state=NORMAL;
}

void ARpcBase::writeMsg(const char *msg,const char *args[],int argsCount)
{
	writeData(msg,strlen(msg));
	for(int i=0;i<argsCount;++i)
	{
		writeCallback("|",1);
		writeData(args[i],strlen(args[i]));
	}
	writeCallback("\n",1);
}

void ARpcBase::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	writeData(msg,strlen(msg));
	if(arg1)
	{
		writeCallback("|",1);
		writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		writeCallback("|",1);
		writeData(arg2,strlen(arg2));
	}
	if(arg3)
	{
		writeCallback("|",1);
		writeData(arg3,strlen(arg3));
	}
	if(arg4)
	{
		writeCallback("|",1);
		writeData(arg4,strlen(arg4));
	}
	writeCallback("\n",1);
}

void ARpcBase::writeData(const char *byteData,unsigned long sz)
{
	char c;
	for(unsigned long i=0;i<sz;++i)
	{
		c=byteData[i];
		if(c==0)
			writeCallback("\\0",2);
		else if(c=='|')
			writeCallback("\\|",2);
		else if(c=='\n')
			writeCallback("\\n",2);
		else if(c=='\\')
			writeCallback("\\\\",2);
		else writeCallback(&c,1);
	}
}

char ARpcBase::charFromHexChar(char c)
{
	if(c>='0'&&c<='9')
		return c-'0';
	else if(c>='a'&&c<='f')
		return c-87;//-'a'+10
	else if(c>='A'&&c<='F')
		return c-55;//-'A'+10
	else return 0;
}
