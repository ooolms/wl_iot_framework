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

#include "ARpcStarNetDevice.h"
#include <string.h>

ARpcStarNet::ARpcStarNet(unsigned long bSize,ARpcWriteCallback wcb1,void *wcbData1,ARpcWriteCallback wcb2,
	void *wcbData2,const ARpcUuid &deviceId,const char *deviceName)
	:parser1(bSize,&ARpcStarNet::msgCallback1,this)
	,parser2(bSize,&ARpcStarNet::msgCallback2,this)
	,writer1(wcb1,wcbData1)
	,writer2(wcb2,wcbData2)
	,writerAny(&ARpcStarNet::writeDataToBothSides,this)
	,msgDisp(deviceId,deviceName,&writerAny)
{
}

void ARpcStarNet::putByte1(char c)
{
	parser1.putByte(c);
}

void ARpcStarNet::putData1(const char *byteData,unsigned long sz)
{
	parser1.putData(byteData,sz);
}

void ARpcStarNet::putByte2(char c)
{
	parser2.putByte(c);
}

void ARpcStarNet::putData2(const char *byteData,unsigned long sz)
{
	parser2.putData(byteData,sz);
}

void ARpcStarNet::installCommandHandler(ARpcMessageCallback ccb,void *ccbData)
{
	msgDisp.installCommandHandler(ccb,ccbData);
}

void ARpcStarNet::writeMsg(const char *msg,const char **args,unsigned char argsCount)
{
	msgDisp.writeMsg(msg,args,argsCount);
}

void ARpcStarNet::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeMsg(msg,arg1,arg2,arg3,arg4);
}

void ARpcStarNet::writeOk(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeOk(arg1,arg2,arg3,arg4);
}

void ARpcStarNet::writeErr(const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeErr(arg1,arg2,arg3,arg4);
}

void ARpcStarNet::writeInfo(const char *info,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	msgDisp.writeInfo(info,arg1,arg2,arg3,arg4);
}

void ARpcStarNet::writeMeasurement(const char *sensor,const char *str)
{
	msgDisp.writeMeasurement(sensor,str);
}

void ARpcStarNet::writeMeasurement(const char *sensor,const char *data,unsigned long dataSize)
{
	msgDisp.writeMeasurement(sensor,data,dataSize);
}

void ARpcStarNet::writeSync()
{
	msgDisp.writeSync();
}

void ARpcStarNet::setControlsInterface(const char *iface)
{
	msgDisp.setControlsInterface(iface);
}

void ARpcStarNet::setSensorsDescription(const char *descr)
{
	msgDisp.setSensorsDescription(descr);
}

void ARpcStarNet::setDestDeviceId(const ARpcUuid &id)
{
	msgDisp.setDestDeviceId(id);
}

void ARpcStarNet::setBroadcast()
{
	msgDisp.setBroadcast();
}

bool ARpcStarNet::processMessage(const char *msg,const char *args[],unsigned char argsCount)
{
	//IMPL check src id and
	ARpcUuid srcId(msg);
	if(!srcId.isValid())return false;
	if(args[0][0]=='#')//reserved messages
	{
		if(strcmp(args[0],"#broadcast")!=0)
			return false;
	}
	else
	{
		ARpcUuid dstId(args[0]);
		if(dstId!=msgDisp.deviceId())return false;
	}
	msgDisp.setDestDeviceId(srcId);
	msgDisp.processMessage(args[1],args+2,argsCount-2);
	return true;
}

void ARpcStarNet::msgCallback1(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
	if(argsCount<2||msg[0]==0||args[0][0]==0||args[1][0]==0)return;
	ARpcStarNet *th=(ARpcStarNet*)data;
	if(!th->processMessage(msg,args,argsCount))
		th->writer2.writeMsg(msg,args,argsCount);
}

void ARpcStarNet::msgCallback2(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
	if(argsCount<2||msg[0]==0||args[0][0]==0||args[1][0]==0)return;
	ARpcStarNet *th=(ARpcStarNet*)data;
	if(!th->processMessage(msg,args,argsCount))
		th->writer1.writeMsg(msg,args,argsCount);
}

void ARpcStarNet::writeDataToBothSides(void *data,const char *str,unsigned long size)
{
	((ARpcStarNet*)data)->writer1.writeDataNoEscape(str,size);
	((ARpcStarNet*)data)->writer2.writeDataNoEscape(str,size);
}
