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

#include "ARpcStarNetDevice.h"
#include "ARpcConfig.h"
#include <string.h>

ARpcStarNetDevice::ARpcStarNetDevice(unsigned long bSize,ARpcIWriteCallback *wcb1,ARpcIWriteCallback *wcb2,
	const ARpcUuid *deviceId,const char *deviceName)
	:writer1Cb(wcb1)
	,writer2Cb(wcb2)
	,writerAnyCb(wcb1,wcb2)
	,msg1Cb(this,0)
	,msg2Cb(this,1)
	,writer1(deviceId,wcb1)
	,writer2(deviceId,wcb2)
	,writerAny(deviceId,&writerAnyCb)
	,parser1(bSize,&msg1Cb)
	,parser2(bSize,&msg2Cb)
	,msgDisp(deviceId,deviceName,&writerAny)
{
}

ARpcStarNetDevice::ARpcStarNetDevice(char *buf1,char *buf2,unsigned long bSize,
	ARpcIWriteCallback *wcb1,ARpcIWriteCallback *wcb2,const ARpcUuid *deviceId,const char *deviceName)
	:writer1Cb(wcb1)
	,writer2Cb(wcb2)
	,writerAnyCb(wcb1,wcb2)
	,msg1Cb(this,0)
	,msg2Cb(this,1)
	,writer1(deviceId,wcb1)
	,writer2(deviceId,wcb2)
	,writerAny(deviceId,&writerAnyCb)
	,parser1(buf1,bSize,&msg1Cb)
	,parser2(buf2,bSize,&msg2Cb)
	,msgDisp(deviceId,deviceName,&writerAny)
{
}

ARpcStarNetDevice::~ARpcStarNetDevice()
{
}

void ARpcStarNetDevice::putByte1(char c)
{
	parser1.putByte(c);
}

void ARpcStarNetDevice::putData1(const char *byteData,unsigned long sz)
{
	parser1.putData(byteData,sz);
}

void ARpcStarNetDevice::putByte2(char c)
{
	parser2.putByte(c);
}

void ARpcStarNetDevice::putData2(const char *byteData,unsigned long sz)
{
	parser2.putData(byteData,sz);
}

void ARpcStarNetDevice::setDestDeviceId(const ARpcUuid &id)
{
	writerAny.setDestId(id);
}

void ARpcStarNetDevice::setBroadcast()
{
	writerAny.setBroadcast();
}

void ARpcStarNetDevice::writeDeviceIdentified()
{
	writerAny.setBroadcast();
	msgDisp.writeMsg("device_identified",msgDisp.deviceName());
}

void ARpcStarNetDevice::processMessage(const ARpcUuid &srcId,const char *msg,const char *args[],unsigned char argsCount)
{
	writerAny.setDestId(srcId);
	msgDisp.processMessage(msg,args,argsCount);
	writerAny.setBroadcast();
}

ARpcStarNetDeviceMsgCallback::ARpcStarNetDeviceMsgCallback(ARpcStarNetDevice *d,char writerNum)
{
	dev=d;
	wNum=writerNum;
	if(wNum==0)//1-st side
		cb=dev->writer2Cb;
	else
		cb=dev->writer1Cb;
	catched=false;
	redirect=true;
}

void ARpcStarNetDeviceMsgCallback::processMessage(const char *msg,const char *args[],unsigned char argsCount)
{
	srcId.parse(msg);
	catched=false;
	redirect=true;
	if(!srcId.isValid())return;
	if(args[0][0]=='#')//reserved messages
	{
		if(strcmp(args[0],bCastMsg)==0)
			catched=true;
	}
	else
	{
		dstId.parse(args[0]);
		if(dstId==(*dev->deviceId()))
		{
			catched=true;
			redirect=false;
		}
	}
	if(catched)
		dev->processMessage(srcId,args[1],args+2,argsCount-2);
	if(redirect)
	{

		cb->writeStr(msg);
		for(unsigned char i=0;i<argsCount;++i)
		{
			cb->writeStr(argDelim);
			cb->writeStr(args[i]);
		}
		cb->writeStr(msgDelim);
	}
}
