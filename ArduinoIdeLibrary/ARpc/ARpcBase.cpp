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

const char delim='|';//код 124
const int maxArgCount=10;//максимальное число аргументов

ARpcBase::ARpcBase(int bSize,ARpcWriteCallback wcb)
{
	writeCallback=wcb;
	bufSize=bSize;
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
	if(bufIndex==bufSize)//переполнение буфера, эпик фейл
	{
		//сбрасываем буфер
		memset(buffer,0,bufSize+1);
		bufIndex=0;
	}
	if(c=='\n')//признак конца сообщения, обрабатываем
	{
		buffer[bufIndex]=0;
		parseMessage();
		memset(buffer,0,bufSize+1);
		bufIndex=0;
	}
	else//продолжаем накапливать буфер
	{
		buffer[bufIndex]=c;
		++bufIndex;
	}
}

//поиск разделителя дальше по строке
int ARpcBase::findDelim(int startFrom)
{
	for(int i=startFrom;i<bufSize;++i)
	{
		if(buffer[i]==0)return -1;
		else if(buffer[i]==delim)return i;
	}
	return -1;
}

//разбор строки из Serial на команду и аргументы
void ARpcBase::parseMessage()
{
	if(buffer[0]==0||buffer[0]==delim)
		return;

	int bufIter=0;
	char *cmd=buffer;
	char *args[maxArgCount];//аргументы
	for(int i=0;i<maxArgCount;++i)
		args[i]=0;
	for(int i=0;i<maxArgCount;++i)
	{
		bufIter=findDelim(bufIter+1);//ищем разделитель
		if(bufIter==-1)//больше нет
		{
			processMessage(cmd,args,i);
			return;
		}
		buffer[bufIter]=0;//заменяем разделитель на символ с кодом 0
		if(bufIter==(bufSize-1))//разделитель в последнем символе в буфере, игнорируем
		{
			processMessage(cmd,args,i);
			return;
		}

		//следующий аргумент будет после позиции разделителя
		++bufIter;
		args[i]=&buffer[bufIter];
	}
}

void ARpcBase::writeMsg(const char *msg,const char *args[],int argsCount)
{
	writeCallback(msg);
	for(int i=0;i<argsCount;++i)
	{
		writeCallback("|");
		writeCallback(args[i]);
	}
	writeCallback("\n");
}

void ARpcBase::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	writeCallback(msg);
	if(arg1)
	{
		writeCallback("|");
		writeCallback(arg1);
	}
	if(arg2)
	{
		writeCallback("|");
		writeCallback(arg2);
	}
	if(arg3)
	{
		writeCallback("|");
		writeCallback(arg3);
	}
	if(arg4)
	{
		writeCallback("|");
		writeCallback(arg4);
	}
	writeCallback("\n");
}
