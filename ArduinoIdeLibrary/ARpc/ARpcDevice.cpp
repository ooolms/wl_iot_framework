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

#include "ARpcDevice.h"

ARpcDevice::ARpcDevice(unsigned long bSize,ARpcIWriteCallback *wcb,
	const ARpcUuid *deviceId,const char *deviceName,bool hub)
	:parser(bSize,static_cast<ARpcIMessageCallback*>(this))
	,writer(wcb)
	,msgDisp(deviceId,deviceName,&writer,hub)
{
}

ARpcDevice::ARpcDevice(char *buf,unsigned long bSize,ARpcIWriteCallback *wcb,
	const ARpcUuid *deviceId,const char *deviceName,bool hub)
	:parser(buf,bSize,static_cast<ARpcIMessageCallback*>(this))
	,writer(wcb)
	,msgDisp(deviceId,deviceName,&writer,hub)
{
}

void ARpcDevice::putByte(char c)
{
	parser.putByte(c);
}

void ARpcDevice::putData(const char *byteData,unsigned long sz)
{
	parser.putData(byteData,sz);
}

void ARpcDevice::reset()
{
	parser.reset();
}

void ARpcDevice::processMsg(const char *msg,const char *args[],unsigned char argsCount)
{
	msgDisp.processMessage(msg,args,argsCount);
}
