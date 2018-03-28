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
#include "ARpcStreamWriter.h"
#include "ArpcBusDeviceMessageDispatch.h"

/*
 * 2-directional network on 2 rs-485 transmitters (for example) or 2 uarts etc.
 * Packet from one side is retransmitted to other side and vice versa.
 * Generated packet in sent to other sides.
*/
class ARpcStarNetDevice
{
public:
	ARpcStarNetDevice(unsigned long bSize,ARpcWriteCallback wcb1,void *wcbData1,ARpcWriteCallback wcb2,void *wcbData2,
		const ARpcUuid &deviceId,const char *deviceName);
	void putByte1(char c);
	void putData1(const char *byteData,unsigned long sz);
	void putByte2(char c);
	void putData2(const char *byteData,unsigned long sz);
	void installCommandHandler(ARpcMessageCallback ccb,void *ccbData);
		//no "call" header
	void writeMsg(const char *msg,const char **args,unsigned char argsCount);
	void writeMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeOk(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeErr(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeInfo(const char *info,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMeasurement(const char *sensor,unsigned char count,const char **args);
	void writeMeasurement(const char *sensor,const char *data,unsigned long dataSize);
	void writeSync();
	void setControlsInterface(const char *iface);// !!! NOT copied
	void setSensorsDescription(const char *descr);// !!! NOT copied
	void setDestDeviceId(const ARpcUuid &id);
	void setBroadcast();
	void writeDeviceIdentified();

private:
	bool processMessage(const char *msg,const char *args[],unsigned char argsCount);//true - catched by itself
	static void msgCallback1(void *data,const char *msg,const char *args[],unsigned char argsCount);
	static void msgCallback2(void *data,const char *msg,const char *args[],unsigned char argsCount);
	static void writeDataToBothSides(void *data,const char *str,unsigned long size);

private:
	ARpcStreamParser parser1,parser2;
	ARpcStreamWriter writer1,writer2,writerAny;
	ArpcBusDeviceMessageDispatch msgDisp;
};

#endif // ARPCSTARNETDEVICE_H
