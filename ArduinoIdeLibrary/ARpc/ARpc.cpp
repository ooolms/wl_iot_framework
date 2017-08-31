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
	:ARpcBase(bSize,wcb)
{
	cmdCallback=ccb;
	writeCallback=wcb;
	cmdReplied=false;
	devId=deviceId;
	devName=deviceName;
	controlInterface=0;
	sensorsDescription=0;
}

ARpc::~ARpc()
{
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
