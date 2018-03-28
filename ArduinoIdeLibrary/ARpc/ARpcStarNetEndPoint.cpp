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
#include <string.h>

ARpcStarNetEndPoint::ARpcStarNetEndPoint(unsigned long bSize,ARpcWriteCallback wcb,void *wcbData,
	ARpcNetMsgCallback msgCb,void *msgCbData,const ARpcUuid &deviceId)
	:devId(deviceId)
	,netParser(bSize,&ARpcStarNetEndPoint::msgCallback,this)
	,netWriter(wcb,wcbData)
{
	messageCallback=msgCb;
	messageCallbackData=msgCbData;
	devId.toHex(idStr);
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

void ARpcStarNetEndPoint::msgCallback(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
	if(argsCount<2||msg[0]==0||args[0][0]==0||args[1][0]==0)return;
	ARpcStarNetEndPoint *th=(ARpcStarNetEndPoint*)data;
	ARpcUuid srcId(msg);
	if(!srcId.isValid())return;
	if(args[0][0]=='#')//reserved messages
	{
		if(strcmp(args[0],ARpcStarNetDevice::bCastMsg)!=0)
			return;
	}
	else
	{
		ARpcUuid dstId(args[0]);
		if(dstId!=th->devId)return;
	}
	th->messageCallback(th->messageCallbackData,srcId,args[1],args+2,argsCount-2);
}

void ARpcStarNetEndPoint::writeMsgToWriter(const char *msg,const char **args,unsigned char argsCount)
{
	netWriter.writeData(msg,strlen(msg));
	for(unsigned char i=0;i<argsCount;++i)
	{
		netWriter.writeDataNoEscape(ARpcStreamWriter::argDelim,1);
		netWriter.writeData(args[i],strlen(args[i]));
	}
	netWriter.writeDataNoEscape(ARpcStreamWriter::msgDelim,1);
}

void ARpcStarNetEndPoint::writeMsgToWriter(
	const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	netWriter.writeData(msg,strlen(msg));
	if(arg1)
	{
		netWriter.writeDataNoEscape(ARpcStreamWriter::argDelim,1);
		netWriter.writeData(arg1,strlen(arg1));
	}
	if(arg2)
	{
		netWriter.writeDataNoEscape(ARpcStreamWriter::argDelim,1);
		netWriter.writeData(arg1,strlen(arg2));
	}
	if(arg3)
	{
		netWriter.writeDataNoEscape(ARpcStreamWriter::argDelim,1);
		netWriter.writeData(arg1,strlen(arg3));
	}
	if(arg4)
	{
		netWriter.writeDataNoEscape(ARpcStreamWriter::argDelim,1);
		netWriter.writeData(arg1,strlen(arg4));
	}
	netWriter.writeDataNoEscape(ARpcStreamWriter::msgDelim,1);
}

void ARpcStarNetEndPoint::writeDevHeader(const ARpcUuid &destId)
{
	char destStr[33];
	destId.toHex(destStr);
	netWriter.writeDataNoEscape(idStr,32);
	netWriter.writeDataNoEscape(ARpcStreamWriter::argDelim,1);
	netWriter.writeDataNoEscape(destStr,32);
	netWriter.writeDataNoEscape(ARpcStreamWriter::argDelim,1);
}

void ARpcStarNetEndPoint::writeBCastHeader()
{
	netWriter.writeDataNoEscape(idStr,32);
	netWriter.writeDataNoEscape(ARpcStreamWriter::argDelim,1);
	netWriter.writeDataNoEscape(ARpcStarNetDevice::bCastMsg,10);
	netWriter.writeDataNoEscape(ARpcStreamWriter::argDelim,1);
}
