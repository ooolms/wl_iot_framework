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

/*
 * 2-directional network on 2 rs-485 transmitters (for example) or 2 uarts etc.
 * Packet from one side is retransmitted to other side and vice versa.
 * Generated packet in sent to other sides.
*/
class ARpcStarNetDevice
{
public:
	ARpcStarNetDevice(unsigned long bSize,ARpcIWriteCallback *wcb1,ARpcIWriteCallback *wcb2,
		const ARpcUuid &deviceId,const char *deviceName);
	~ARpcStarNetDevice();
	void putByte1(char c);
	void putData1(const char *byteData,unsigned long sz);
	void putByte2(char c);
	void putData2(const char *byteData,unsigned long sz);
	void installCommandHandler(ARpcIMessageCallback *ccb);
		//no "call" header
	void writeMsg(const char *msg,const char **args,unsigned char argsCount);
	void writeMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeOk(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeErr(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeInfo(const char *arg1,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMeasurement(const char *sensor,const char *val);
	void writeMeasurementF(const char *sensor,const float &val);
	void writeMeasurement(const char *sensor,unsigned char count,const char **args);
	void writeMeasurement(const char *sensor,const char *data,unsigned long dataSize);
	void writeSync();
	void setControls(const char *controls);// !!! NOT copied
	void setSensors(const char *sensors);// !!! NOT copied
	void setDestDeviceId(const ARpcUuid &id);
	void setBroadcast();
	void writeDeviceIdentified();
	inline const ARpcUuid& deviceId(){return msgDisp->deviceId();}

private:
	void writeDataToBothSides(const char *str,unsigned long size);
	void processMessage(const ARpcUuid &srcId,const char *msg,const char *args[],unsigned char argsCount);

private:
	ARpcStreamParser *parser1,*parser2;
	ARpcStarNetStreamWriter *writer1,*writer2,*writerAny;
	ARpcIWriteCallback *writer1Cb,*writer2Cb,*writerAnyCb;
	ARpcIMessageCallback *msg1Cb,*msg2Cb;
	ARpcRealDeviceMessageDispatch *msgDisp;
	friend class ARpcStarNetDeviceMsgCallback;
};

#endif // ARPCSTARNETDEVICE_H
