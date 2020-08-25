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

#ifndef DEVICE_H
#define DEVICE_H

#include "wliot/simple/StreamWriter.h"
#include "wliot/simple/StreamParser.h"
#include "wliot/simple/Uuid.h"
#include "wliot/simple/RealDeviceMessageDispatch.h"
#include "wliot/simple/DeviceState.h"

class Device
	:public IMessageCallback
{
public:
	explicit Device(unsigned long bSize,IWriteCallback *wcb,
		const Uuid *deviceId,const char *deviceName,bool hub=false);
	explicit Device(char *buf,unsigned long bSize,IWriteCallback *wcb,
		const Uuid *deviceId,const char *deviceName,bool hub=false);
	// !!! deviceName is NOT copied (mem economy)
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void resetStream();
	void resetParser();
	inline RealDeviceMessageDispatch& disp(){return msgDisp;}
	virtual void processMsg(const char *msg,const char **args,unsigned char argsCount)override;

private:
	StreamParser parser;
	StreamWriter writer;
	RealDeviceMessageDispatch msgDisp;
};

#endif
