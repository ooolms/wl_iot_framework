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

#ifndef ARPCSTARNETDEVICE_H
#define ARPCSTARNETDEVICE_H

#include "ARpcStreamParser.h"
#include "ARpcStarNetStreamWriter.h"
#include "ARpcRealDeviceMessageDispatch.h"

class ARpcStarNetDevice;

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
	ARpcStarNetDeviceMsgCallback(ARpcStarNetDevice *d,char writerNum);
	virtual void processMsg(const char *msg,const char *args[],unsigned char argsCount)override;

private:
	ARpcStarNetDevice *dev;
	char wNum;
	ARpcUuid srcId,dstId;
	ARpcStarNetStreamWriter *wr;
	bool catched,redirect;
};

/*
 * 2-directional network on 2 rs-485 transmitters (for example) or 2 uarts etc.
 * Packet from one side is retransmitted to other side and vice versa.
 * Generated packet in sent to other sides.
*/
class ARpcStarNetDevice
{
public:
	ARpcStarNetDevice(unsigned long bSize,ARpcIWriteCallback *wcb1,ARpcIWriteCallback *wcb2,
		const ARpcUuid *deviceId,const char *deviceName);
	ARpcStarNetDevice(char *buf1,char *buf2,unsigned long bSize,ARpcIWriteCallback *wcb1,ARpcIWriteCallback *wcb2,
		const ARpcUuid *deviceId,const char *deviceName);
	~ARpcStarNetDevice();
	void putByte1(char c);
	void putData1(const char *byteData,unsigned long sz);
	void putByte2(char c);
	void putData2(const char *byteData,unsigned long sz);
	void setDestDeviceId(const ARpcUuid &id);
	void setBroadcast();
	void writeDeviceIdentified();
	inline ARpcRealDeviceMessageDispatch& disp(){return msgDisp;}
	inline const ARpcUuid* deviceId(){return msgDisp.deviceId();}

private:
	void writeDataToBothSides(const char *str,unsigned long size);
	void processMessage(const ARpcUuid &srcId,const char *msg,const char *args[],unsigned char argsCount);

private:
	ARpcIWriteCallback *writer1Cb,*writer2Cb;
	ARpcStarNetDevice2SidesWriter writerAnyCb;
	ARpcStarNetDeviceMsgCallback msg1Cb,msg2Cb;
	ARpcStarNetStreamWriter writer1,writer2,writerAny;
	ARpcStreamParser parser1,parser2;
	ARpcRealDeviceMessageDispatch msgDisp;
	friend class ARpcStarNetDeviceMsgCallback;
};

#endif // ARPCSTARNETDEVICE_H
