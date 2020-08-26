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
#include "wliot/simple/IConnectionCallback.h"
#include "wliot/simple/DeviceState.h"
#include "wliot/simple/ClientSocket.h"

class Device
	:public IMessageCallback
	,public IDevEventsCallback
{
public:
	explicit Device(unsigned long bSize,const Uuid *deviceId,const char *deviceName,bool hub=false);
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void resetStream();
	void resetParser();
	void setConnectionCallback(IConnectionCallback *cb);
	void setDevEventsCallback(IDevEventsCallback *cb);
	inline RealDeviceMessageDispatch& disp(){return msgDisp;}
	virtual void processMsg(const char *msg,const char **args,unsigned char argsCount)override;
	void onConnected();
	void onDisconnected();
	void onNewData(const char *data,size_t sz);
	ClientSocket& sock();
	virtual void processCommand(const char *cmd, const char **args, unsigned char argsCount) override;
	virtual void onSyncMsg() override;
	virtual void onFirstSetupCmd(const Uuid &uuid, const char *name) override;
	void checkTimer();

private:
	ClientSocket mSock;
	int mSyncTimer;
	StreamParser parser;
	StreamWriter writer;
	RealDeviceMessageDispatch msgDisp;
	IConnectionCallback *sockCb;
	IDevEventsCallback *devEvCb;
};

#endif
