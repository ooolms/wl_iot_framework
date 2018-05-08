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

#ifndef ARPCREALDEVICEMESSAGEDISPATCH_H
#define ARPCREALDEVICEMESSAGEDISPATCH_H

#include "ARpcUuid.h"
#include "ARpcDeviceState.h"
#include "ARpcStreamParser.h"
#include "ARpcStreamWriter.h"

class ARpcRealDeviceMessageDispatch
{
private:
	ARpcRealDeviceMessageDispatch(const ARpcRealDeviceMessageDispatch &);

public:
	explicit ARpcRealDeviceMessageDispatch(
		const ARpcUuid &deviceId,const char *deviceName,ARpcStreamWriter *p,bool hub=false);
	~ARpcRealDeviceMessageDispatch();
	ARpcDeviceState *state();
	void installCommandHandler(ARpcIMessageCallback *ccb);
		//no "call" header
	void installHubMsgHandler(ARpcIMessageCallback *hcb);
		//no "#hub" header, msg is dest address, args[0] is a message itself
	void writeMsg(const char *msg,const char **args,unsigned char argsCount);
	void writeMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMsgFromHub(const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount);
	void writeOk(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeErr(const char *arg1,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeInfo(const char *arg1,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMeasurement(const char *sensor,const char *val1,const char *val2=0,const char *val3=0);
	void writeMeasurement(const char *sensor,unsigned char count,const char **args);
	void writeMeasurement(const char *sensor,const char *data,unsigned long dataSize);
	void writeMeasurementF(const char *sensor,const float &v);
	void writeMeasurementF(const char *sensor,const float *v,unsigned long count);
	void writeSync();
	void setControls(const char *controls);// !!! NOT copied
	void setSensors(const char *sensors);// !!! NOT copied
	ARpcStreamWriter* writer();
	void processMessage(const char *msg,const char *args[],unsigned char argsCount);
	const ARpcUuid& deviceId();
	const char* deviceName();

protected:
	ARpcStreamWriter *mWriter;
	ARpcUuid devId;
	const char *devName;
	bool isHub;

private:
	ARpcIMessageCallback *cmdCallback;
	ARpcIMessageCallback *hubMsgCallback;
	bool cmdReplied;
	const char *controlInterface;
	const char *sensorsDescription;
	ARpcDeviceState *mState;
};

#endif // ARPCREALDEVICEMESSAGEDISPATCH_H