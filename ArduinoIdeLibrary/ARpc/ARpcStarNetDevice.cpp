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
#include <string.h>

const char *ARpcStarNetDevice::bCastMsg="#broadcast";

class ARpcStarNetDeviceMsgCallback
{
public:
	explicit ARpcStarNetDeviceMsgCallback(ARpcStarNetDevice *d);

private:
	ARpcStarNetDevice *device;
};

class ARpcStarNetDeviceWriter
	:public ARpcIWriteCallback
{
public:
	explicit ARpcStarNetDeviceWriter(ARpcStarNetDevice *d);

	virtual void writeData(const char *data,unsigned long *sz)override
	{

	}

	virtual void writeStr(const char *str)override
	{

	}

private:
	ARpcStarNetDevice *dev;
};

ARpcStarNetDevice::ARpcStarNetDevice(unsigned long bSize,ARpcIWriteCallback *wcb1,ARpcIWriteCallback *wcb2,
	const ARpcUuid &deviceId,const char *deviceName)
	:parser1(bSize,&ARpcStarNetDevice::msgCallback1)
	,parser2(bSize,&ARpcStarNetDevice::msgCallback2)
	,msgDisp(deviceId,deviceName,&writerAny)
{
	writer1Cb=wcb1;
	writer2Cb=wcb2;
	writerAnyCb=new ARpcStarNetDeviceMsgCallback(this);
	msgCb=new ARpcStarNetDeviceMsgCallback(this);

	writer1=new ARpcStreamWriter(wcb1);
	writer2=new ARpcStreamWriter(wcb2);
	writerAny=new ARpcStreamWriter(writerAnyCb);

	parser1=new ARpcStreamParser(bSize,msgCb);
	parser2=new ARpcStreamParser(bSize,msgCb);
	msgDisp=new ARpcBusDeviceMessageDispatch(deviceId,deviceName,writerAny);
}

ARpcStarNetDevice::~ARpcStarNetDevice()
{
	delete msgDisp;
	delete parser1;
	delete parser2;

	delete writerAny;
	delete writer1;
	delete writer2;

	delete writerAnyCb;
	delete msgCb;
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

void ARpcStarNetDevice::installCommandHandler(ARpcIMessageCallback *ccb)
{
	msgDisp.installCommandHandler(ccb);
}

void ARpcStarNetDevice::writeMsg(const char *msg,const char **args,unsigned char argsCount)
{
	msgDisp.writeMsg(msg,args,argsCount);
}

void ARpcStarNetDevice::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeMsg(msg,arg1,arg2,arg3,arg4);
}

void ARpcStarNetDevice::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeOk(arg1,arg2,arg3,arg4);
}

void ARpcStarNetDevice::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeErr(arg1,arg2,arg3,arg4);
}

void ARpcStarNetDevice::writeInfo(const char *info,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeInfo(info,arg1,arg2,arg3,arg4);
}

void ARpcStarNetDevice::writeMeasurement(const char *sensor,const char *val)
{
	msgDisp.writeMeasurement(sensor,val);
}

void ARpcStarNetDevice::writeMeasurement(const char *sensor,unsigned char count,const char **args)
{
	msgDisp.writeMeasurement(sensor,count,args);
}

void ARpcStarNetDevice::writeMeasurement(const char *sensor,const char *data,unsigned long dataSize)
{
	msgDisp.writeMeasurement(sensor,data,dataSize);
}

void ARpcStarNetDevice::writeSync()
{
	msgDisp.writeSync();
}

void ARpcStarNetDevice::setControls(const char *controls)
{
	msgDisp.setControls(controls);
}

void ARpcStarNetDevice::setSensors(const char *sensors)
{
	msgDisp.setSensors(sensors);
}

void ARpcStarNetDevice::setDestDeviceId(const ARpcUuid &id)
{
	msgDisp.setDestDeviceId(id);
}

void ARpcStarNetDevice::setBroadcast()
{
	msgDisp.setBroadcast();
}

void ARpcStarNetDevice::writeDeviceIdentified()
{
	msgDisp.setBroadcast();
	msgDisp.writeMsg("device_identified",msgDisp.deviceName());
}

bool ARpcStarNetDevice::processMessage(const char *msg,const char *args[],unsigned char argsCount)
{
	ARpcUuid srcId(msg);
	bool catched=false;
	if(!srcId.isValid())
		return false;
	if(args[0][0]=='#')//reserved messages
	{
		if(strcmp(args[0],bCastMsg)!=0)
			return false;
	}
	else
	{
		ARpcUuid dstId(args[0]);
		if(!(dstId==msgDisp.deviceId()))
			return false;
		catched=true;
	}
	msgDisp.setDestDeviceId(srcId);
	msgDisp.processMessage(args[1],args+2,argsCount-2);
	return catched;
}

void ARpcStarNetDevice::msgCallback1(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
	if(argsCount<2||msg[0]==0||args[0][0]==0||args[1][0]==0)
		return;
	ARpcStarNetDevice *th=(ARpcStarNetDevice*)data;
	if(!th->processMessage(msg,args,argsCount))
		th->writer2.writeMsg(msg,args,argsCount);
}

void ARpcStarNetDevice::msgCallback2(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
	if(argsCount<2||msg[0]==0||args[0][0]==0||args[1][0]==0)return;
	ARpcStarNetDevice *th=(ARpcStarNetDevice*)data;
	if(!th->processMessage(msg,args,argsCount))
		th->writer1.writeMsg(msg,args,argsCount);
}

void ARpcStarNetDevice::writeDataToBothSides(const char *str,unsigned long size)
{
	(str,size);
	writer2.writeDataNoEscape(str,size);
}
