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
#include "ARpcConfig.h"
#include <string.h>
#include <stdlib.h>

const int maxArgCount=10;//максимальное число аргументов

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
}

ARpcRealDeviceMessageDispatch::~ARpcRealDeviceMessageDispatch()
{
	delete mState;
}

void ARpcRealDeviceMessageDispatch::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	if(!mWriter->beginWriteMsg())return;
	cmdReplied=true;
	mWriter->writeArgNoEscape(okMsg);
	if(arg1)
		mWriter->writeArg(arg1,strlen(arg1));
	if(arg2)
		mWriter->writeArg(arg2,strlen(arg2));
	if(arg3)
		mWriter->writeArg(arg3,strlen(arg3));
	if(arg4)
		mWriter->writeArg(arg4,strlen(arg4));
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	if(!mWriter->beginWriteMsg())return;
	cmdReplied=true;
	mWriter->writeArgNoEscape(errMsg);
	if(arg1)
		mWriter->writeArg(arg1,strlen(arg1));
	if(arg2)
		mWriter->writeArg(arg2,strlen(arg2));
	if(arg3)
		mWriter->writeArg(arg3,strlen(arg3));
	if(arg4)
		mWriter->writeArg(arg4,strlen(arg4));
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::writeInfo(
	const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape(infoMsg);
	if(arg1)
		mWriter->writeArg(arg1,strlen(arg1));
	if(arg2)
		mWriter->writeArg(arg2,strlen(arg2));
	if(arg3)
		mWriter->writeArg(arg3,strlen(arg3));
	if(arg4)
		mWriter->writeArg(arg4,strlen(arg4));
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::writeMeasurement(
	const char *sensor,const char *val1,const char *val2,const char *val3)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape(measurementMsg);
	mWriter->writeArg(sensor,strlen(sensor));
	mWriter->writeArg(val1,strlen(val1));
	if(val2)
		mWriter->writeArg(val2,strlen(val2));
	if(val3)
		mWriter->writeArg(val3,strlen(val3));
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::writeMeasurementF(const char *sensor,const float &v)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape(measurementFMsg);
	mWriter->writeArg(sensor,strlen(sensor));
	mWriter->writeArg((const char*)&v,4);
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::writeMeasurementF(const char *sensor,const float *v,unsigned long count)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape(measurementFMsg);
	mWriter->writeArg(sensor,strlen(sensor));
	mWriter->writeArg((const char*)v,4*count);
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::writeMeasurement(const char *sensor,unsigned char count,const char **args)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape(measurementMsg);
	mWriter->writeArg(sensor,strlen(sensor));
	for(unsigned char i=0;i<count;++i)
		mWriter->writeArg(args[i],strlen(args[i]));
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::writeMeasurement(const char *sensor,const char *data,unsigned long dataSize)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape(measurementMsg);
	mWriter->writeArg(sensor,strlen(sensor));
	mWriter->writeArg(data,dataSize);
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::writeSync()
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape(syncMsg);
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::setControls(const char *controls)
{
	controlInterface=controls;
}

void ARpcRealDeviceMessageDispatch::setSensors(const char *sensors)
{
	sensorsDescription=sensors;
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
		hubMsgCallback->processMessage(args[0],args+1,argsCount-1);
	}
	else if(strcmp(msg,"identify")==0)
	{
		char str[40];
		devId.toString(str);
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
		hubMsgCallback->processMessage(bCastMsg,&msg,1);
		writeOk();
	}
	else if(strcmp(msg,"queryversion")==0)
		mWriter->writeMsg("version","simple_v1.0");
	else if(strcmp(msg,"call")==0)
	{
		if(argsCount<1||strlen(args[0])==0)
			writeErr("No command");
		else if(args[0][0]=='#')
		{
			if(strcmp(args[0],"#sensors")==0)
			{
				if(sensorsDescription)
					writeOk(sensorsDescription);
				else writeOk("");
			}
			else if(strcmp(args[0],"#controls")==0)
			{
				if(controlInterface)writeOk(controlInterface);
				else writeOk("");
			}
			else if(strcmp(args[0],"#state")==0)
			{
				if(!mWriter->beginWriteMsg())return;
				mWriter->writeArgNoEscape(okMsg);
				mState->dump();
				mWriter->endWriteMsg();
			}
			else writeErr("bad system command");
		}
		else
		{
			cmdReplied=false;
			if(cmdCallback)
				cmdCallback->processMessage(args[0],&args[1],argsCount-1);
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

ARpcDeviceState* ARpcRealDeviceMessageDispatch::state()
{
	return mState;
}

void ARpcRealDeviceMessageDispatch::installHubMsgHandler(ARpcIMessageCallback *hcb)
{
	hubMsgCallback=hcb;
}

void ARpcRealDeviceMessageDispatch::writeMsg(const char *msg,const char **args,unsigned char argsCount)
{
	mWriter->writeMsg(msg,args,argsCount);
}

void ARpcRealDeviceMessageDispatch::writeMsg(const char *msg,
	const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	mWriter->writeMsg(msg,arg1,arg2,arg3,arg4);
}

void ARpcRealDeviceMessageDispatch::writeMsgFromHub(
	const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount)
{
	mWriter->beginWriteMsg();
	mWriter->writeArgNoEscape("#hub");
	char src[33];
	srcId.toHex(src);
	mWriter->writeArgNoEscape(src);
	mWriter->writeArg(msg,strlen(msg));
	for(unsigned char i=0;i<argsCount;++i)
		mWriter->writeArg(args[i],strlen(args[i]));
	mWriter->endWriteMsg();
}

void ARpcRealDeviceMessageDispatch::installCommandHandler(ARpcIMessageCallback *ccb)
{
	cmdCallback=ccb;
}
