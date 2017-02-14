/*******************************************
This file is part of ArduinoRpc. ArduinoRpc is a simple text
communication protocol for Arduino boards.
Copyright (C) 2016 Denis Bespalov <master1312@yandex.ru>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "ARpc.h"

const char delim='|';//код 124
const int maxArgCount=10;//максимальное число аргументов
const char *ARpc::okMsg="ok";
const char *ARpc::errMsg="err";
const char *ARpc::infoMsg="info";
const char *ARpc::measurementMsg="meas";
const char *ARpc::syncMsg="sync";

ARpc::ARpc(int bSize,ARpcCommandCallback ccb,ARpcWriteCallback wcb,const char *devId,const char *devName)
{
	bufSize=bSize;
	buffer=(char*)malloc(bufSize+1);
	memset(buffer,0,bufSize+1);
	cmdCallback=ccb;
	writeCallback=wcb;
	bufIndex=0;
	if(devId)
	{
		deviceId=malloc(strlen(devId)+1);
		strcpy(deviceId,devId);
	}
	else deviceId=0;
	if(devName)
	{
		deviceName=malloc(strlen(devName)+1);
		strcpy(deviceName,devName);
	}
	else deviceName=0;
}

ARpc::~ARpc()
{
	free(buffer);
	if(deviceId)free(deviceId);
	if(deviceName)free(deviceName);
}

void ARpc::putChar(char c)
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
		parseCommand();
		memset(buffer,0,bufSize+1);
		bufIndex=0;
	}
	else//продолжаем накапливать буфер
	{
		buffer[bufIndex]=c;
		++bufIndex;
	}
}

//обработка команд
void ARpc::processCommand(char *cmd,char *args[],int argsCount)
{
	if(strcmp(cmd,"identify")==0)
	{
		if(deviceId&&deviceName)
			writeMsg("deviceinfo",deviceId,deviceName);
		else writeErr("No id or name set");
	}
	cmdCallback((const char *)cmd,(const char **)args,argsCount,this);
}

//поиск разделителя дальше по строке
int ARpc::findDelim(int startFrom)
{
	for(int i=startFrom;i<bufSize;++i)
	{
		if(buffer[i]==0)return -1;
		else if(buffer[i]==delim)return i;
	}
	return -1;
}

//разбор строки из Serial на команду и аргументы
void ARpc::parseCommand()
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
			processCommand(cmd,args,i);
			return;
		}
		buffer[bufIter]=0;//заменяем разделитель на символ с кодом 0
		if(bufIter==(bufSize-1))//разделитель в последнем символе в буфере, игнорируем
		{
			processCommand(cmd,args,i);
			return;
		}

		//следующий аргумент будет после позиции разделителя
		++bufIter;
		args[i]=&buffer[bufIter];
	}
}

void ARpc::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	writeCallback(okMsg);
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

void ARpc::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	writeCallback(errMsg);
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

void ARpc::writeInfo(const char *info,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	writeCallback(infoMsg);
	if(info)
	{
		writeCallback("|");
		writeCallback(info);
	}
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

void ARpc::writeMsg(const char *msg,const char *args[],int argsCount)
{
	writeCallback(msg);
	for(int i=0;i<argsCount;++i)
	{
		writeCallback("|");
		writeCallback(args[i]);
	}
	writeCallback("\n");
}

void ARpc::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
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

void ARpc::writeMeasurement(const char *sensor,const char *value)
{
	writeCallback(measurementMsg);
	writeCallback("|");
	writeCallback(sensor);
	writeCallback("|");
	writeCallback(value);
	writeCallback("\n");
}

void ARpc::writeSync()
{
	writeCallback(syncMsg);
	writeCallback("\n");
}

