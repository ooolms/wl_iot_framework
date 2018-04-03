/*******************************************
Copyright 2018 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ARpcStreamParser.h"
#include "ARpcUuid.h"
#include <string.h>
#include <stdlib.h>

ARpcStreamParser::ARpcStreamParser(unsigned long bSize,ARpcMessageCallback mcb,void *mcbData)
{
	messageCallback=mcb;
	messageCallbackData=mcbData;
	bufSize=bSize;
	currentArgIndex=0;
	buffer=(char*)malloc(bufSize+1);
	memset(buffer,0,bufSize+1);
	bufIndex=0;
	state=NORMAL;
	hexChars[0]=hexChars[1]=0;
	args[0]=0;
}

ARpcStreamParser::~ARpcStreamParser()
{
	free(buffer);
}

void ARpcStreamParser::putByte(char c)
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
		hexChars[0]=ARpcUuid::byteFromHexChar(hexChars[0]);
		hexChars[1]=ARpcUuid::byteFromHexChar(hexChars[1]);
		if(hexChars[0]!=0xff&&hexChars[1]!=0xff)
		{
			buffer[bufIndex]=(char)((hexChars[0]<<4)+hexChars[1]);
			++bufIndex;
		}
	}
	else //NORMAL
		parseCharInNormalState(c);
	if(bufIndex==bufSize)//переполнение буфера, эпик фейл
		reset();
}

void ARpcStreamParser::putData(const char *byteData,unsigned long sz)
{
	for(unsigned long i=0;i<sz;++i)
		putByte(byteData[i]);
}

void ARpcStreamParser::reset()
{
	bufIndex=0;
	buffer[0]=0;
	currentArgIndex=0;
}

void ARpcStreamParser::parseCharInNormalState(char c)
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
		buffer[bufIndex]=0;
		if(messageCallback)
			messageCallback(messageCallbackData,buffer,(const char**)args,currentArgIndex);
		reset();
	}
	else
	{
		buffer[bufIndex]=c;
		++bufIndex;
	}
}

void ARpcStreamParser::parseCharInEscapeState(char c)
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
