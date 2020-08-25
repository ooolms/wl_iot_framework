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

#include "wliot/simple/RealDeviceMessageDispatch.h"
#include <stdlib.h>
#include <string.h>

const int maxArgCount=10;//максимальное число аргументов
static const char *emptyCallId="";

RealDeviceMessageDispatch::RealDeviceMessageDispatch(
	const Uuid *deviceId,const char *deviceName,StreamWriter *p,bool hub)
	:devId(deviceId)
	,mState(p)
{
	isHub=hub;
	devName=deviceName;
	mWriter=p;
	controlInterface=0;
	sensorsDescription=0;
	eventsCallback=0;
	hubMsgCallback=0;
	cmdReplied=false;
}

RealDeviceMessageDispatch::~RealDeviceMessageDispatch()
{
}

void RealDeviceMessageDispatch::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	if(!mWriter->beginWriteMsg())return;
	cmdReplied=true;
	mWriter->writeArgNoEscape("ok");
	mWriter->writeArg(callIdStr,strlen(callIdStr));
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

void RealDeviceMessageDispatch::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	if(!mWriter->beginWriteMsg())return;
	cmdReplied=true;
	mWriter->writeArgNoEscape("err");
	mWriter->writeArg(callIdStr,strlen(callIdStr));
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

void RealDeviceMessageDispatch::writeInfo(
	const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape("info");
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

void RealDeviceMessageDispatch::writeMeasurement(
	const char *sensor,const char *val1,const char *val2,const char *val3)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape("meas");
	mWriter->writeArg(sensor,strlen(sensor));
	mWriter->writeArg(val1,strlen(val1));
	if(val2)
		mWriter->writeArg(val2,strlen(val2));
	if(val3)
		mWriter->writeArg(val3,strlen(val3));
	mWriter->endWriteMsg();
}

void RealDeviceMessageDispatch::writeMeasurement(const char *sensor,unsigned char count,const char **args)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape("meas");
	mWriter->writeArg(sensor,strlen(sensor));
	for(unsigned char i=0;i<count;++i)
		mWriter->writeArg(args[i],strlen(args[i]));
	mWriter->endWriteMsg();
}

void RealDeviceMessageDispatch::writeMeasurement(const char *sensor,const char *data,unsigned long dataSize)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape("meas");
	mWriter->writeArg(sensor,strlen(sensor));
	mWriter->writeArg(data,dataSize);
	mWriter->endWriteMsg();
}

void RealDeviceMessageDispatch::writeMeasurementB(const char *sensor,const unsigned char *v,unsigned long count)
{
	writeMeasurementBImpl(sensor,(const char *)v,count,sizeof(unsigned char));
}

void RealDeviceMessageDispatch::writeMeasurementB(const char *sensor,const unsigned short *v,unsigned long count)
{
	writeMeasurementBImpl(sensor,(const char *)v,count,sizeof(unsigned short));
}

void RealDeviceMessageDispatch::writeMeasurementB(const char *sensor,const unsigned long *v,unsigned long count)
{
	writeMeasurementBImpl(sensor,(const char *)v,count,sizeof(unsigned long));
}

void RealDeviceMessageDispatch::writeMeasurementB(const char *sensor,const float *v,unsigned long count)
{
	writeMeasurementBImpl(sensor,(const char *)v,count,sizeof(float));
}

void RealDeviceMessageDispatch::writeSyncr()
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape("syncr");
	mWriter->endWriteMsg();
}

void RealDeviceMessageDispatch::setControls(const char *controls)
{
	controlInterface=controls;
}

void RealDeviceMessageDispatch::setSensors(const char *sensors)
{
	sensorsDescription=sensors;
}

StreamWriter* RealDeviceMessageDispatch::writer()
{
	return mWriter;
}

//CRIT support for all known messages
void RealDeviceMessageDispatch::processMessage(const char *msg,const char **args,unsigned char argsCount)
{
	if(strcmp(msg,"#hub")==0&&hubMsgCallback)
	{
		if(argsCount<2||!hubMsgCallback)return;
		hubMsgCallback->processMsg(args[0],args+1,argsCount-1);
	}
	else if(strcmp(msg,"identify")==0)
	{
		char str[40];
		devId->toString(str);
		mWriter->beginWriteMsg();
		mWriter->writeArgNoEscape("deviceinfo");
		if(isHub)mWriter->writeArgNoEscape("#hub");
		mWriter->writeArgNoEscape(str);
		mWriter->writeArg(devName,strlen(devName));
		mWriter->endWriteMsg();
	}
	else if(strcmp(msg,"identify_hub")==0)
	{
		if(!hubMsgCallback)
		{
			writeErrNoEscape("not a hub device");
			return;
		}
		const char *msg="identify";
		hubMsgCallback->processMsg("#broadcast",&msg,1);
		writeOk();
	}
//	else if(strcmp(msg,"queryversion")==0)
//		mWriter->writeMsg("version","simple_v1.1");
	else if(strcmp(msg,"sync")==0)
	{
		if(eventsCallback)
			eventsCallback->onSyncMsg();
		writeSyncr();
	}
	else if(strcmp(msg,"call")==0)
	{
		callIdStr=emptyCallId;
		if(argsCount<2||strlen(args[0])==0||strlen(args[1])==0)
		{
			writeErrNoEscape("No command or call id");
			return;
		}
		callIdStr=args[0];
		if(args[1][0]=='#')
		{
			if(strcmp(args[1],"#sensors")==0)
			{
				if(sensorsDescription)
					writeOk(sensorsDescription);
				else writeOkNoEscape("");
			}
			else if(strcmp(args[1],"#controls")==0)
			{
				if(controlInterface)
					writeOk(controlInterface);
				else writeOkNoEscape("");
			}
			else if(strcmp(args[1],"#state")==0)
			{
				if(!mWriter->beginWriteMsg())return;
				mWriter->writeArgNoEscape("ok");
				mWriter->writeArg(callIdStr,strlen(callIdStr));
				mState.dump();
				mWriter->endWriteMsg();
			}
			else if(strcmp(args[1],"#setup")==0)
			{
				if(argsCount<4)
				{
					writeErrNoEscape("bad setup command");
					return;
				}
				const Uuid uuid(args[2]);
				const char *name=args[3];
				if(!uuid.isValid())
				{
					writeErrNoEscape("bad setup command");
					return;
				}
				if(eventsCallback)
					eventsCallback->onFirstSetupCmd(uuid,name);
				writeOk();
			}
			else writeErrNoEscape("bad system command");
		}
		else
		{
			cmdReplied=false;
			if(eventsCallback)
				eventsCallback->processCommand(args[1],args+2,argsCount-2);
			if(!cmdReplied)
				writeErrNoEscape("unknown command");
		}
	}
	/*else if(strcmp(msg,"ok")==0||strcmp(msg,"err")==0)
	{
		//TODO calls processing?
	}*/
}

const Uuid* RealDeviceMessageDispatch::deviceId()
{
	return devId;
}

const char* RealDeviceMessageDispatch::deviceName()
{
	return devName;
}

void RealDeviceMessageDispatch::beginWriteOk()
{
	if(!mWriter->beginWriteMsg())return;
	cmdReplied=true;
	mWriter->writeArgNoEscape("ok");
	mWriter->writeArg(callIdStr,strlen(callIdStr));
}

void RealDeviceMessageDispatch::writeOkArg(const char *arg)
{
	mWriter->writeArg(arg,strlen(arg));
}

void RealDeviceMessageDispatch::endWriteOk()
{
	mWriter->endWriteMsg();
}

void RealDeviceMessageDispatch::writeMeasurementBImpl(
	const char *sensor,const char *v,unsigned long count,unsigned char sizeofV)
{
	if(!mWriter->beginWriteMsg())return;
	mWriter->writeArgNoEscape("measb");
	mWriter->writeArg(sensor,strlen(sensor));
	mWriter->writeArg((const char*)v,count*sizeofV);
	mWriter->endWriteMsg();
}

void RealDeviceMessageDispatch::writeOkNoEscape(const char *str)
{
	if(!mWriter->beginWriteMsg())return;
	cmdReplied=true;
	mWriter->writeArgNoEscape("ok");
	mWriter->writeArg(callIdStr,strlen(callIdStr));
	mWriter->writeArgNoEscape(str);
	mWriter->endWriteMsg();
}

void RealDeviceMessageDispatch::writeErrNoEscape(const char *str)
{
	if(!mWriter->beginWriteMsg())return;
	cmdReplied=true;
	mWriter->writeArgNoEscape("err");
	mWriter->writeArg(callIdStr,strlen(callIdStr));
	mWriter->writeArgNoEscape(str);
	mWriter->endWriteMsg();
}

DeviceState* RealDeviceMessageDispatch::state()
{
	return &mState;
}

void RealDeviceMessageDispatch::installHubMsgHandler(IMessageCallback *hcb)
{
	hubMsgCallback=hcb;
}

void RealDeviceMessageDispatch::writeMsg(const char *msg,const char **args,unsigned char argsCount)
{
	mWriter->writeMsg(msg,args,argsCount);
}

void RealDeviceMessageDispatch::writeMsg(const char *msg,
	const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	mWriter->writeMsg(msg,arg1,arg2,arg3,arg4);
}

void RealDeviceMessageDispatch::writeMsgFromHub(
	const Uuid &srcId,const char *msg,const char **args,unsigned char argsCount)
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

void RealDeviceMessageDispatch::installDevEventsHandler(IDevEventsCallback *cb)
{
	eventsCallback=cb;
}
