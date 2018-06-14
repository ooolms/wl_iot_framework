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

#include "ARpcStarNetEndPoint.h"
#include "ARpcStarNetDevice.h"
#include "ARpcConfig.h"
#include <string.h>

ARpcStarNetEndPoint::ARpcStarNetEndPoint(unsigned long bSize,ARpcIWriteCallback *wcb,
	ARpcINetMsgCallback *msgCb,const ARpcUuid *deviceId)
	:devId(deviceId)
	,netParser(bSize,static_cast<ARpcIMessageCallback*>(this))
	,netWriter(wcb)
{
	messageCallback=msgCb;
	devId->toHex(idStr);
}

ARpcStarNetEndPoint::ARpcStarNetEndPoint(char *buf,unsigned long bSize,ARpcIWriteCallback *wcb,
	ARpcINetMsgCallback *msgCb,const ARpcUuid *deviceId)
	:devId(deviceId)
	,netParser(buf,bSize,static_cast<ARpcIMessageCallback*>(this))
	,netWriter(wcb)
{
	messageCallback=msgCb;
	devId->toHex(idStr);
}

void ARpcStarNetEndPoint::putByte(char c)
{
	netParser.putByte(c);
}

void ARpcStarNetEndPoint::putData(const char *byteData,unsigned long sz)
{
	netParser.putData(byteData,sz);
}

void ARpcStarNetEndPoint::writeMsg(const ARpcUuid &destId,const char *msg,const char **args,unsigned char argsCount)
{
	writeDevHeader(destId);
	writeMsgToWriter(msg,args,argsCount);
}

void ARpcStarNetEndPoint::writeMsg(const ARpcUuid &destId,const char *msg,
	const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	writeDevHeader(destId);
	writeMsgToWriter(msg,arg1,arg2,arg3,arg4);
}

void ARpcStarNetEndPoint::writeBCastMsg(const char *msg,const char **args,unsigned char argsCount)
{
	writeBCastHeader();
	writeMsgToWriter(msg,args,argsCount);
}

void ARpcStarNetEndPoint::writeBCastMsg(const char *msg,const char *arg1,
	const char *arg2,const char *arg3,const char *arg4)
{
	writeBCastHeader();
	writeMsgToWriter(msg,arg1,arg2,arg3,arg4);
}

void ARpcStarNetEndPoint::processMsg(const char *msg,const char *args[],unsigned char argsCount)
{
	if(argsCount<2||msg[0]==0||args[0][0]==0||args[1][0]==0)return;
	ARpcUuid srcId(msg);
	if(!srcId.isValid())return;
	if(args[0][0]=='#')//reserved messages
	{
		if(strcmp(args[0],bCastMsg)!=0)
			return;
	}
	else
	{
		ARpcUuid dstId(args[0]);
		if(dstId!=*devId)return;
	}
	messageCallback->processMsg(srcId,args[1],args+2,argsCount-2);
}

void ARpcStarNetEndPoint::writeMsgToWriter(const char *msg,const char **args,unsigned char argsCount)
{
	netWriter.writeArg(msg,strlen(msg));
	for(unsigned char i=0;i<argsCount;++i)
		netWriter.writeArg(args[i],strlen(args[i]));
	netWriter.endWriteMsg();
}

void ARpcStarNetEndPoint::writeMsgToWriter(
	const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	netWriter.writeArg(msg,strlen(msg));
	if(arg1)
		netWriter.writeArg(arg1,strlen(arg1));
	if(arg2)
		netWriter.writeArg(arg2,strlen(arg2));
	if(arg3)
		netWriter.writeArg(arg3,strlen(arg3));
	if(arg4)
		netWriter.writeArg(arg4,strlen(arg4));
	netWriter.endWriteMsg();
}

void ARpcStarNetEndPoint::writeDevHeader(const ARpcUuid &destId)
{
	if(!netWriter.beginWriteMsg())return;
	char destStr[33];
	destId.toHex(destStr);
	netWriter.writeArgNoEscape(idStr);
	netWriter.writeArgNoEscape(destStr);
}

void ARpcStarNetEndPoint::writeBCastHeader()
{
	if(!netWriter.beginWriteMsg())return;
	netWriter.writeArgNoEscape(idStr);
	netWriter.writeArgNoEscape(bCastMsg);
}
