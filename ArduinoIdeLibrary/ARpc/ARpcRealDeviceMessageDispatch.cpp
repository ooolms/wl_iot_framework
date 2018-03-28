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

ARpcRealDeviceMessageDispatch::ARpcRealDeviceMessageDispatch(
	const ARpcUuid &deviceId,const char *deviceName,ARpcStreamWriter *p,bool hub)
	:devId(deviceId)
{
	isHub=hub;
	devName=deviceName;
	mWriter=p;
	controlInterface=0;
	sensorsDescription=0;
	mState=new ARpcDeviceState(this);
	cmdCallback=0;
	hubMsgCallback=0;
	cmdReplied=false;
	cmdCallbackData=hubMsgCallbackData=0;
}

ARpcRealDeviceMessageDispatch::~ARpcRealDeviceMessageDispatch()
{
	delete mState;
}

void ARpcRealDeviceMessageDispatch::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	cmdReplied=true;
	beginWriteMessage();
	mWriter->writeDataNoEscape(okMsg,strlen(okMsg));
	if(arg1)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg2,strlen(arg2));
	}
	if(arg3)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg3,strlen(arg3));
	}
	if(arg4)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg4,strlen(arg4));
	}
	mWriter->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	cmdReplied=true;
	beginWriteMessage();
	mWriter->writeDataNoEscape(errMsg,strlen(errMsg));
	if(arg1)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg2,strlen(arg2));
	}
	if(arg3)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg3,strlen(arg3));
	}
	if(arg4)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg4,strlen(arg4));
	}
	mWriter->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeInfo(const char *info,const char *arg1,const char *arg2,
	const char *arg3,const char *arg4)
{
	beginWriteMessage();
	mWriter->writeDataNoEscape(infoMsg,strlen(infoMsg));
	if(info)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(info,strlen(info));
	}
	if(arg1)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg2,strlen(arg2));
	}
	if(arg3)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg3,strlen(arg3));
	}
	if(arg4)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg4,strlen(arg4));
	}
	mWriter->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeMeasurement(const char *sensor,unsigned char count,const char **args)
{
	beginWriteMessage();
	mWriter->writeDataNoEscape(measurementMsg,strlen(measurementMsg));
	mWriter->writeDataNoEscape("|",1);
	mWriter->writeData(sensor,strlen(sensor));
	for(unsigned char i=0;i<count;++i)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(args[i],strlen(args[i]));
	}
	mWriter->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeMeasurement(const char *sensor,const char *data,unsigned long dataSize)
{
	beginWriteMessage();
	mWriter->writeDataNoEscape(measurementMsg,strlen(measurementMsg));
	mWriter->writeDataNoEscape("|",1);
	mWriter->writeData(sensor,strlen(sensor));
	mWriter->writeDataNoEscape("|",1);
	mWriter->writeData(data,dataSize);
	mWriter->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeSync()
{
	beginWriteMessage();
	mWriter->writeDataNoEscape(syncMsg,strlen(syncMsg));
	mWriter->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::setControlsInterface(const char *iface)
{
	controlInterface=iface;
}

void ARpcRealDeviceMessageDispatch::setSensorsDescription(const char *descr)
{
	sensorsDescription=descr;
}

ARpcStreamWriter* ARpcRealDeviceMessageDispatch::writer()
{
	return mWriter;
}

//CRIT support for all known messages
void ARpcRealDeviceMessageDispatch::processMessage(const char *msg,const char *args[],unsigned char argsCount)
{
	if(strcmp(msg,"#hub")==0&&hubMsgCallback)
	{
		if(argsCount<2||!hubMsgCallback)return;
		beginWriteMessage();
		hubMsgCallback(hubMsgCallbackData,args[0],args+1,argsCount-1);
	}
	else if(strcmp(msg,"identify")==0)
	{
		char str[40];
		devId.toString(str);
		beginWriteMessage();
		if(isHub)mWriter->writeMsg("deviceinfo","#hub",str,devName);
		else mWriter->writeMsg("deviceinfo",str,devName);
	}
	else if(strcmp(msg,"identify_hub")==0)
	{
		if(!hubMsgCallback)
		{
			writeErr("not a hub device");
			return;
		}
		const char *msg="identify";
		hubMsgCallback(hubMsgCallbackData,"#broadcast",&msg,1);
		writeOk();
	}
	else if(strcmp(msg,"queryversion")==0)
	{
		beginWriteMessage();
		mWriter->writeMsg("version","simple_v1.0");
	}
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
				beginWriteMessage();
				mWriter->writeDataNoEscape(okMsg,strlen(okMsg));
				mState->dump();
				mWriter->writeDataNoEscape("\n",1);
			}
		}
		else
		{
			cmdReplied=false;
			if(cmdCallback)
				cmdCallback(cmdCallbackData,(const char *)args[0],(const char **)(&args[1]),argsCount-1);
			if(!cmdReplied)writeErr("unknown command");
		}
	}
	else if(strcmp(msg,"ok")==0||strcmp(msg,"err")==0)
	{
		//TODO calls processing?
	}
}

const ARpcUuid& ARpcRealDeviceMessageDispatch::deviceId()
{
	return devId;
}

const char* ARpcRealDeviceMessageDispatch::deviceName()
{
	return devName;
}

void ARpcRealDeviceMessageDispatch::beginWriteMessage()
{
}

ARpcDeviceState* ARpcRealDeviceMessageDispatch::state()
{
	return mState;
}

void ARpcRealDeviceMessageDispatch::installHubMsgHandler(ARpcMessageCallback hcb,void *hcbData)
{
	hubMsgCallback=hcb;
	hubMsgCallbackData=hcbData;
}

void ARpcRealDeviceMessageDispatch::writeMsg(const char *msg, const char **args, unsigned char argsCount)
{
	beginWriteMessage();
	mWriter->writeData(msg,strlen(msg));
	for(unsigned char i=0;i<argsCount;++i)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(args[i],strlen(args[i]));
	}
	mWriter->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeMsg(const char *msg,
	const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	beginWriteMessage();
	mWriter->writeData(msg,strlen(msg));
	if(arg1)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg2,strlen(arg2));
	}
	if(arg3)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg3,strlen(arg3));
	}
	if(arg4)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(arg4,strlen(arg4));
	}
	mWriter->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::writeMsgFromHub(
	const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount)
{
	mWriter->writeDataNoEscape("#hub",4);
	mWriter->writeDataNoEscape("|",1);
	char src[33];
	srcId.toHex(src);
	mWriter->writeDataNoEscape(src,32);
	mWriter->writeDataNoEscape("|",1);
	mWriter->writeData(msg,strlen(msg));
	for(unsigned char i=0;i<argsCount;++i)
	{
		mWriter->writeDataNoEscape("|",1);
		mWriter->writeData(args[i],strlen(args[i]));
	}
	mWriter->writeDataNoEscape("\n",1);
}

void ARpcRealDeviceMessageDispatch::installCommandHandler(ARpcMessageCallback ccb,void *ccbData)
{
	cmdCallback=ccb;
	cmdCallbackData=ccbData;
}
