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
#include "ARpcUuid.h"

class ARpcINetMsgCallback
{
public:
	virtual void processMsg(const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount)=0;
};

/*
 * 2-directional network on 2 rs-485 transmitters (for example) or 2 uarts etc.
 * Packet from one side is retransmitted to other side and vice versa.
 * Generated packet in sent to other sides.
*/
class ARpcStarNetEndPoint
	:public ARpcIMessageCallback
{
public:
	ARpcStarNetEndPoint(unsigned long bSize,ARpcIWriteCallback *wcb,
		ARpcINetMsgCallback *msgCb,const ARpcUuid &deviceId);
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void writeMsg(const ARpcUuid &destId,const char *msg,const char **args,unsigned char argsCount);
	void writeMsg(const ARpcUuid &destId,const char *msg,
		const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeBCastMsg(const char *msg,const char **args,unsigned char argsCount);
	void writeBCastMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	virtual void processMessage(const char *msg, const char *args[], unsigned char argsCount) override;

private:
	void writeMsgToWriter(const char *msg,const char **args,unsigned char argsCount);
	void writeMsgToWriter(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
	void writeDevHeader(const ARpcUuid &destId);
	void writeBCastHeader();

private:
	char idStr[33];
	ARpcUuid devId;
	ARpcStreamParser netParser;
	ARpcStreamWriter netWriter;
	ARpcINetMsgCallback *messageCallback;
};

#endif // ARPCSTARNETENDPOINT_H
