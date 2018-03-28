/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef APRCDEVICE_H
#define APRCDEVICE_H

#include "ARpcStreamWriter.h"
#include "ARpcStreamParser.h"
#include "ARpcUuid.h"
#include "ARpcRealDeviceMessageDispatch.h"
#include "ARpcDeviceState.h"

class ARpcDevice
{
public:
	explicit ARpcDevice(unsigned long bSize,ARpcWriteCallback wcb,void *wcbData,
		const ARpcUuid &deviceId,const char *deviceName,bool hub=false);
	// !!! deviceName is NOT copied (mem economy)
	ARpcDeviceState *state();
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void reset();
	void installCommandHandler(ARpcMessageCallback ccb,void *ccbData);
	void installHubMsgHandler(ARpcMessageCallback hcb,void *hcbData);
	void writeMsg(const char *msg,const char **args,unsigned char argsCount);
	void writeMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMsgFromHub(const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount);
	void writeOk(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeErr(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeInfo(const char *info,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMeasurement(const char *sensor,const char *val);
	void writeMeasurement(const char *sensor,unsigned char count,const char **args);
	void writeMeasurement(const char *sensor,const char *data,unsigned long dataSize);
	void writeSync();
	void setControlsInterface(const char *iface);// !!! NOT copied
	void setSensorsDescription(const char *descr);// !!! NOT copied

private:
	static void msgCallback(void *data,const char *msg,const char *args[],unsigned char argsCount);

private:
	ARpcStreamParser parser;
	ARpcStreamWriter writer;
	ARpcRealDeviceMessageDispatch msgDisp;
};

#endif
