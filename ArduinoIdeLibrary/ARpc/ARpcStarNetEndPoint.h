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

#ifndef ARPCSTARNETENDPOINT_H
#define ARPCSTARNETENDPOINT_H

#include "ARpcStreamParser.h"
#include "ARpcStreamWriter.h"
#include "ArpcBusDeviceMessageDispatch.h"

typedef void (*ARpcNetMsgCallback)(
	void *data,const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount);

/*
 * 2-directional network on 2 rs-485 transmitters (for example) or 2 uarts etc.
 * Packet from one side is retransmitted to other side and vice versa.
 * Generated packet in sent to other sides.
*/
class ARpcStarNetEndPoint
{
public:
	ARpcStarNetEndPoint(unsigned long bSize,ARpcWriteCallback wcb,void *wcbData,
		ARpcNetMsgCallback msgCb,void *msgCbData,const ARpcUuid &deviceId);
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void writeMsg(const ARpcUuid &destId,const char *msg,const char **args,unsigned char argsCount);
	void writeMsg(const ARpcUuid &destId,const char *msg,
		const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeBCastMsg(const char *msg,const char **args,unsigned char argsCount);
	void writeBCastMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);

private:
	static void msgCallback(void *data,const char *msg,const char *args[],unsigned char argsCount);
	void writeMsgToWriter(const char *msg,const char **args,unsigned char argsCount);
	void writeMsgToWriter(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
	void writeDevHeader(const ARpcUuid &destId);
	void writeBCastHeader();

private:
	char idStr[33];
	ARpcUuid devId;
	ARpcStreamParser netParser;
	ARpcStreamWriter netWriter;
	ARpcNetMsgCallback messageCallback;
	void *messageCallbackData;
};

#endif // ARPCSTARNETENDPOINT_H
