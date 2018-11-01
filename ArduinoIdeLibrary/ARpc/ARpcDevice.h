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
	:public ARpcIMessageCallback
{
public:
	explicit ARpcDevice(unsigned long bSize,ARpcIWriteCallback *wcb,
		const ARpcUuid *deviceId,const char *deviceName,bool hub=false);
	explicit ARpcDevice(char *buf,unsigned long bSize,ARpcIWriteCallback *wcb,
		const ARpcUuid *deviceId,const char *deviceName,bool hub=false);
	// !!! deviceName is NOT copied (mem economy)
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void resetStream();
	void resetParser();
	inline ARpcRealDeviceMessageDispatch& disp(){return msgDisp;}
	virtual void processMsg(const char *msg,const char **args,unsigned char argsCount)override;

private:
	ARpcStreamParser parser;
	ARpcStreamWriter writer;
	ARpcRealDeviceMessageDispatch msgDisp;
};

#endif
