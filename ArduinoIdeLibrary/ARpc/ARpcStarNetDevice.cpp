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

class ARpcStarNetDevice2SidesWriter
	:public ARpcIWriteCallback
{
public:
	explicit ARpcStarNetDevice2SidesWriter(ARpcIWriteCallback *w1,ARpcIWriteCallback *w2)
	{
		wr1=w1;
		wr2=w2;
	}

	virtual void writeData(const char *data,unsigned long sz)override
	{
		wr1->writeData(data,sz);
		wr2->writeData(data,sz);
	}

	virtual void writeStr(const char *str)override
	{
		wr1->writeStr(str);
		wr2->writeStr(str);
	}

private:
	ARpcIWriteCallback *wr1,*wr2;
};

class ARpcStarNetDeviceMsgCallback
	:public ARpcIMessageCallback
{
public:
	ARpcStarNetDeviceMsgCallback(ARpcStarNetDevice *d,char writerNum)
	{
		dev=d;
		wNum=writerNum;
	}

	virtual void processMessage(const char *msg,const char *args[],unsigned char argsCount)override
	{
		ARpcUuid srcId(msg);
		bool catched=false,redirect=true;
		if(!srcId.isValid())return;
		if(args[0][0]=='#')//reserved messages
		{
			if(strcmp(args[0],bCastMsg)==0)
				catched=true;
		}
		else
		{
			ARpcUuid dstId(args[0]);
			if(dstId==dev->deviceId())
			{
				catched=true;
				redirect=false;
			}
		}
		if(catched)
			dev->processMessage(srcId,args[1],args+2,argsCount-2);
		if(redirect)
		{
			ARpcIWriteCallback *cb;
			if(wNum==0)//1-st side
				cb=dev->writer2Cb;
			else
				cb=dev->writer1Cb;
			cb->writeStr(msg);
			for(unsigned char i=0;i<argsCount;++i)
			{
				cb->writeStr(argDelim);
				cb->writeStr(args[i]);
			}
			cb->writeStr(msgDelim);
		}
	}

private:
	ARpcStarNetDevice *dev;
	char wNum;
};

ARpcStarNetDevice::ARpcStarNetDevice(unsigned long bSize,ARpcIWriteCallback *wcb1,ARpcIWriteCallback *wcb2,
	const ARpcUuid &deviceId,const char *deviceName)
{
	writer1Cb=wcb1;
	writer2Cb=wcb2;
	writerAnyCb=new ARpcStarNetDevice2SidesWriter(wcb1,wcb2);

	writer1=new ARpcStarNetStreamWriter(deviceId,wcb1);
	writer2=new ARpcStarNetStreamWriter(deviceId,wcb2);
	writerAny=new ARpcStarNetStreamWriter(deviceId,writerAnyCb);

	msg1Cb=new ARpcStarNetDeviceMsgCallback(this,0);
	msg2Cb=new ARpcStarNetDeviceMsgCallback(this,1);
	parser1=new ARpcStreamParser(bSize,msg1Cb);
	parser2=new ARpcStreamParser(bSize,msg2Cb);
	msgDisp=new ARpcRealDeviceMessageDispatch(deviceId,deviceName,writerAny);
}

ARpcStarNetDevice::~ARpcStarNetDevice()
{
	delete msgDisp;
	delete parser1;
	delete parser2;

	delete msg1Cb;
	delete msg2Cb;

	delete writerAny;
	delete writer1;
	delete writer2;

	delete writerAnyCb;
}

void ARpcStarNetDevice::putByte1(char c)
{
	parser1->putByte(c);
}

void ARpcStarNetDevice::putData1(const char *byteData,unsigned long sz)
{
	parser1->putData(byteData,sz);
}

void ARpcStarNetDevice::putByte2(char c)
{
	parser2->putByte(c);
}

void ARpcStarNetDevice::putData2(const char *byteData,unsigned long sz)
{
	parser2->putData(byteData,sz);
}

void ARpcStarNetDevice::installCommandHandler(ARpcIMessageCallback *ccb)
{
	msgDisp->installCommandHandler(ccb);
}

void ARpcStarNetDevice::writeMsg(const char *msg,const char **args,unsigned char argsCount)
{
	msgDisp->writeMsg(msg,args,argsCount);
}

void ARpcStarNetDevice::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp->writeMsg(msg,arg1,arg2,arg3,arg4);
}

void ARpcStarNetDevice::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp->writeOk(arg1,arg2,arg3,arg4);
}

void ARpcStarNetDevice::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp->writeErr(arg1,arg2,arg3,arg4);
}

void ARpcStarNetDevice::writeInfo(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp->writeInfo(arg1,arg2,arg3,arg4);
}

void ARpcStarNetDevice::writeMeasurement(const char *sensor,const char *val)
{
	msgDisp->writeMeasurement(sensor,val);
}

void ARpcStarNetDevice::writeMeasurementF(const char *sensor,const float &val)
{
	msgDisp->writeMeasurementF(sensor,val);
}

void ARpcStarNetDevice::writeMeasurement(const char *sensor,unsigned char count,const char **args)
{
	msgDisp->writeMeasurement(sensor,count,args);
}

void ARpcStarNetDevice::writeMeasurement(const char *sensor,const char *data,unsigned long dataSize)
{
	msgDisp->writeMeasurement(sensor,data,dataSize);
}

void ARpcStarNetDevice::writeSync()
{
	msgDisp->writeSync();
}

void ARpcStarNetDevice::setControls(const char *controls)
{
	msgDisp->setControls(controls);
}

void ARpcStarNetDevice::setSensors(const char *sensors)
{
	msgDisp->setSensors(sensors);
}

void ARpcStarNetDevice::setDestDeviceId(const ARpcUuid &id)
{
	writerAny->setDestId(id);
}

void ARpcStarNetDevice::setBroadcast()
{
	writerAny->setBroadcast();
}

void ARpcStarNetDevice::writeDeviceIdentified()
{
	writerAny->setBroadcast();
	msgDisp->writeMsg("device_identified",msgDisp->deviceName());
}

void ARpcStarNetDevice::processMessage(const ARpcUuid &srcId,const char *msg,const char *args[],unsigned char argsCount)
{
	writerAny->setDestId(srcId);
	msgDisp->processMessage(msg,args,argsCount);
	writerAny->setBroadcast();
}