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

#ifndef REALDEVICEMESSAGEDISPATCH_H
#define REALDEVICEMESSAGEDISPATCH_H

#include "wliot/simple/Uuid.h"
#include "wliot/simple/DeviceState.h"
#include "wliot/simple/StreamParser.h"
#include "wliot/simple/StreamWriter.h"
#include "wliot/simple/IDevEventsCallback.h"

class RealDeviceMessageDispatch
{
private:
	RealDeviceMessageDispatch(const RealDeviceMessageDispatch &);

public:
	explicit RealDeviceMessageDispatch(
		const Uuid *deviceId,const char *deviceName,StreamWriter *p,IDevEventsCallback *cb,bool hub=false);
	~RealDeviceMessageDispatch();
	DeviceState *state();
	void setHubMsgHandler(IMessageCallback *hcb);
		//no "#hub" header, msg is dest address, args[0] is a message itself
	void writeMsg(const char *msg,const char **args,unsigned char argsCount);
	void writeMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMsgFromHub(const Uuid &srcId,const char *msg,const char **args,unsigned char argsCount);
	void writeOk(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeErr(const char *arg1,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeInfo(const char *arg1,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMeasurement(const char *sensor,const char *val1,const char *val2=0,const char *val3=0);
	void writeMeasurement(const char *sensor,unsigned char count,const char **args);
	void writeMeasurement(const char *sensor,const char *data,unsigned long dataSize);
	void writeMeasurementB(const char *sensor,const unsigned char *v,unsigned long count);
	void writeMeasurementB(const char *sensor,const unsigned short *v,unsigned long count);
	void writeMeasurementB(const char *sensor,const unsigned long *v,unsigned long count);
	void writeMeasurementB(const char *sensor,const float *v,unsigned long count);
	void writeMeasurementB(const char *sensor,const double *v,unsigned long count);
	void writeSyncr();
	void setControls(const char *controls);
	void setSensors(const char *sensors);
	StreamWriter* writer();
	void processMessage(const char *msg,const char **args,unsigned char argsCount);
	const Uuid* deviceId();
	const char* deviceName();
	void beginWriteOk();
	void writeOkArg(const char *arg);
	void endWriteOk();

private:
	void writeMeasurementBImpl(const char *sensor,const char *v,unsigned long count,unsigned char sizeofV);
	void writeOkNoEscape(const char *str);
	void writeErrNoEscape(const char *str);

protected:
	StreamWriter *mWriter;
	const Uuid *devId;
	const char *devName;
	bool isHub;

private:
	IDevEventsCallback *eventsCallback;
	IMessageCallback *hubMsgCallback;
	bool cmdReplied;
	const char *controlInterface;
	const char *sensorsDescription;
	const char *callIdStr;
	DeviceState mState;
};

#endif // REALDEVICEMESSAGEDISPATCH_H
