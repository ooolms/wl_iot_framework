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

#include "ARpcRealDeviceMessageDispatch.h"
#include "ARpcStreamWriter.h"
#include <string.h>
#include <stdlib.h>

const char delim='|';//код 124
const int maxArgCount=10;//максимальное число аргументов
const char *ARpcRealDeviceMessageDispatch::okMsg="ok";
const char *ARpcRealDeviceMessageDispatch::errMsg="err";
const char *ARpcRealDeviceMessageDispatch::infoMsg="info";
const char *ARpcRealDeviceMessageDispatch::measurementMsg="meas";
const char *ARpcRealDeviceMessageDispatch::syncMsg="sync";

ARpcRealDeviceMessageDispatch::ARpcRealDeviceMessageDispatch(const char *deviceId,const char *deviceName,ARpcStreamWriter *p)
	:devId(deviceId)
{
	devName=deviceName;
	mParser=p;
	controlInterface=0;
	sensorsDescription=0;
	mState=new ARpcDeviceState(this);
}

ARpcRealDeviceMessageDispatch::~ARpcRealDeviceMessageDispatch()
{
	delete mState;
}

void ARpcRealDeviceMessageDispatch::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	cmdReplied=true;
	mParser->writeDataNoEscape(okMsg,strlen(okMsg));
	if(arg1)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg2,strlen(arg2));
	}
	if(arg3)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg3,strlen(arg3));
	}
	if(arg4)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg4,strlen(arg4));
	}
	mParser->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	cmdReplied=true;
	mParser->writeDataNoEscape(errMsg,strlen(errMsg));
	if(arg1)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg2,strlen(arg2));
	}
	if(arg3)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg3,strlen(arg3));
	}
	if(arg4)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg4,strlen(arg4));
	}
	mParser->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeInfo(const char *info,const char *arg1,const char *arg2,
	const char *arg3,const char *arg4)
{
	mParser->writeDataNoEscape(infoMsg,strlen(infoMsg));
	if(info)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(info,strlen(info));
	}
	if(arg1)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg2,strlen(arg2));
	}
	if(arg3)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg3,strlen(arg3));
	}
	if(arg4)
	{
		mParser->writeDataNoEscape("|",1);
		mParser->writeData(arg4,strlen(arg4));
	}
	mParser->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeMeasurement(const char *sensor, const char *str)
{
	mParser->writeDataNoEscape(measurementMsg,strlen(measurementMsg));
	mParser->writeDataNoEscape("|",1);
	mParser->writeData(sensor,strlen(sensor));
	mParser->writeDataNoEscape("|",1);
	mParser->writeData(str,strlen(str));
	mParser->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeMeasurement(const char *sensor,const char *data,unsigned long dataSize)
{
	mParser->writeDataNoEscape(measurementMsg,strlen(measurementMsg));
	mParser->writeDataNoEscape("|",1);
	mParser->writeData(sensor,strlen(sensor));
	mParser->writeDataNoEscape("|",1);
	mParser->writeData(data,dataSize);
	mParser->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeSync()
{
	mParser->writeDataNoEscape(syncMsg,strlen(syncMsg));
	mParser->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::setControlsInterface(const char *iface)
{
	controlInterface=iface;
}

void ARpcRealDeviceMessageDispatch::setSensorsDescription(const char *descr)
{
	sensorsDescription=descr;
}

ARpcStreamWriter* ARpcRealDeviceMessageDispatch::parser()
{
	return mParser;
}

void ARpcRealDeviceMessageDispatch::processMessage(const char *msg,const char *args[], unsigned char argsCount)
{
	if(strcmp(msg,"identify")==0)
	{
		char str[40];
		devId.toString(str);
		mParser->writeMsg("deviceinfo",str,devName);
	}
	else if(strcmp(msg,"queryversion")==0)
		mParser->writeMsg("version","simple_v1.0");
	else if(strcmp(msg,"call")==0)
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
			else if(strcmp(args[0],"#state")==0)
			{
				mParser->writeDataNoEscape(okMsg,strlen(okMsg));
				mState->dump();
				mParser->writeDataNoEscape("\n",1);
			}
		}
		else
		{
			cmdReplied=false;
			if(cmdCallback)
				cmdCallback(cmdCallbackData,(const char *)args[0],(const char **)(&args[1]),argsCount-1);
			if(!cmdReplied)writeErr("unknown error");
		}
	}
	else writeInfo("ERROR: unknown message");
}

const ARpcUuid& ARpcRealDeviceMessageDispatch::deviceId()
{
	return devId;
}

ARpcDeviceState* ARpcRealDeviceMessageDispatch::state()
{
	return mState;
}

void ARpcRealDeviceMessageDispatch::installCommandHandler(ARpcCommandCallback ccb, void *ccbData)
{
	cmdCallback=ccb;
	cmdCallbackData=ccbData;
}
