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
	const ARpcUuid &deviceId,const char *deviceName,bool hub)
	:parser(bSize,static_cast<ARpcIMessageCallback*>(this))
	,writer(wcb)
	,msgDisp(deviceId,deviceName,&writer,hub)
{
}

ARpcDeviceState *ARpcDevice::state()
{
	return msgDisp.state();
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

void ARpcDevice::installCommandHandler(ARpcIMessageCallback *ccb)
{
	msgDisp.installCommandHandler(ccb);
}

void ARpcDevice::installHubMsgHandler(ARpcIMessageCallback *hcb)
{
	msgDisp.installHubMsgHandler(hcb);
}

void ARpcDevice::writeMsg(const char *msg,const char **args,unsigned char argsCount)
{
	msgDisp.writeMsg(msg,args,argsCount);
}

void ARpcDevice::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeMsg(msg,arg1,arg2,arg3,arg4);
}

void ARpcDevice::writeMsgFromHub(const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount)
{
	msgDisp.writeMsgFromHub(srcId,msg,args,argsCount);
}

void ARpcDevice::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeOk(arg1,arg2,arg3,arg4);
}

void ARpcDevice::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeErr(arg1,arg2,arg3,arg4);
}

void ARpcDevice::writeInfo(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeInfo(arg1,arg2,arg3,arg4);
}

void ARpcDevice::writeMeasurement(const char *sensor,const char *val)
{
	msgDisp.writeMeasurement(sensor,val);
}

void ARpcDevice::writeMeasurement(const char *sensor,unsigned char count,const char **args)
{
	msgDisp.writeMeasurement(sensor,count,args);
}

void ARpcDevice::writeMeasurement(const char *sensor,const char *data,unsigned long dataSize)
{
	msgDisp.writeMeasurement(sensor,data,dataSize);
}

void ARpcDevice::writeSync()
{
	msgDisp.writeSync();
}

void ARpcDevice::setControls(const char *controls)
{
	msgDisp.setControls(controls);
}

void ARpcDevice::setSensors(const char *sensors)
{
	msgDisp.setSensors(sensors);
}

void ARpcDevice::processMessage(const char *msg,const char *args[],unsigned char argsCount)
{
	msgDisp.processMessage(msg,args,argsCount);
}
