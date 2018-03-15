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

#ifndef APRC_H
#define APRC_H

#include "ARpcBase.h"
#include "ARpcDeviceState.h"

class ARpc;
typedef void (*ARpcCommandCallback)(const char *cmd,const char *args[],unsigned char argsCount,ARpc *parser);
//write data to other side

class ARpc
	:public ARpcBase
{
public:
	explicit ARpc(unsigned long bSize,ARpcCommandCallback ccb,ARpcWriteCallback wcb,
		const char *deviceId,const char *deviceName);
	// !!! deviceId and deviceName are NOT copied (mem economy)
	virtual ~ARpc();
	void writeOk(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeErr(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeInfo(const char *info,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMeasurement(const char *sensor,const char *str);
	void writeMeasurement(const char *sensor,const char *data,unsigned long dataSize);
	void writeSync();
	void setControlsInterface(const char *iface);// !!! NOT copied
	void setSensorsDescription(const char *descr);// !!! NOT copied
	ARpcDeviceState *state();
	//TODO func calls

protected:
	virtual void processMessage(char *cmd,char *args[],unsigned char argsCount);

public:
	static const char *okMsg;
	static const char *errMsg;
	static const char *infoMsg;
	static const char *measurementMsg;
	static const char *syncMsg;

private:
	friend class ARpcDeviceState;
	bool cmdReplied;
	const char *devId,*devName;
	const char *controlInterface;
	const char *sensorsDescription;
	ARpcCommandCallback cmdCallback;
	ARpcDeviceState *mState;
};

#endif
