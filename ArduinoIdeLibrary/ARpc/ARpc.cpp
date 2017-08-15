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

#include "ARpc.h"
#include <string.h>
#include <stdlib.h>

const char delim='|';//код 124
const int maxArgCount=10;//максимальное число аргументов
const char *ARpc::okMsg="ok";
const char *ARpc::errMsg="err";
const char *ARpc::infoMsg="info";
const char *ARpc::measurementMsg="meas";
const char *ARpc::syncMsg="sync";

ARpc::ARpc(int bSize,ARpcCommandCallback ccb,ARpcWriteCallback wcb,const char *deviceId,const char *deviceName)
{
	bufSize=bSize;
	buffer=(char*)malloc(bufSize+1);
	memset(buffer,0,bufSize+1);
	cmdCallback=ccb;
	writeCallback=wcb;
	bufIndex=0;
	cmdReplied=false;
	devId=deviceId;
	devName=deviceName;
	controlInterface=0;
	sensorsDescription=0;
}

ARpc::~ARpc()
{
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
void ARpc::processMessage(char *cmd,char *args[],int argsCount)
{
	if(strcmp(cmd,"identify")==0)
		writeMsg("deviceinfo",devId,devName);
	else if(strcmp(cmd,"queryversion")==0)
		writeMsg("version","simple_v1.0");
	else if(strcmp(cmd,"call")==0)
	{
		if(argsCount<1||strlen(args[0])==0)
			writeErr("No command");
		else if(args[0][0]=='#')
		{
			if(strcmp(args[0],"#sensors")==0)
			{
				if(sensorsDescription)writeOk(sensorsDescription);
				else writeOk("");
			}
			else if(strcmp(args[0],"#controls")==0)
			{
				if(controlInterface)writeOk(controlInterface);
				else writeOk("");
			}
		}
		else
		{
			cmdReplied=false;
			cmdCallback((const char *)args[0],(const char **)(&args[1]),argsCount-1,this);
			if(!cmdReplied)writeErr("unknown error");
		}
	}
	else writeInfo("ERROR: unknown message");
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

void ARpc::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	cmdReplied=true;
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
	cmdReplied=true;
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

void ARpc::setControlsInterface(const char *iface)
{
	controlInterface=iface;
}

void ARpc::setSensorsDescription(const char *descr)
{
	sensorsDescription=descr;
}

