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

#include "wliot/simple/Device.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdio.h>

static itimerspec ts={{12,0},{12,0}};

Device::Device(unsigned long bSize,const Uuid *deviceId,const char *deviceName,bool hub)
	:mSock(this)
	,parser(bSize,static_cast<IMessageCallback*>(this))
	,writer(static_cast<IWriteCallback*>(&mSock))
	,msgDisp(deviceId,deviceName,&writer,static_cast<IDevEventsCallback*>(this),hub)
{
	sockCb=0;
	devEvCb=0;
	mSyncTimer=-1;
}

void Device::putByte(char c)
{
	parser.putByte(c);
}

void Device::putData(const char *byteData,unsigned long sz)
{
	parser.putData(byteData,sz);
}

void Device::resetStream()
{
	writer.resetStream();
}

void Device::resetParser()
{
	parser.reset();
}

void Device::setConnectionCallback(IConnectionCallback *cb)
{
	sockCb=cb;
}

void Device::setDevEventsCallback(IDevEventsCallback *cb)
{
	devEvCb=cb;
}

void Device::processMsg(const char *msg,const char **args,unsigned char argsCount)
{
	msgDisp.processMessage(msg,args,argsCount);
}

void Device::onConnected()
{
	mSyncTimer=timerfd_create(CLOCK_REALTIME,TFD_NONBLOCK);
	if(mSyncTimer==-1)return;
	timerfd_settime(mSyncTimer,0,&ts,0);
	if(sockCb)
		sockCb->onConnected();
}

void Device::onDisconnected()
{
	close(mSyncTimer);
	mSyncTimer=-1;
	if(sockCb)
		sockCb->onDisconnected();
}

void Device::onNewData(const char *data,size_t sz)
{
	parser.putData(data,sz);
}

ClientSocket& Device::sock()
{
	return mSock;
}

void Device::processCommand(const char *cmd,const char **args,unsigned char argsCount)
{
	if(devEvCb)
		devEvCb->processCommand(cmd,args,argsCount);
}

void Device::onSyncMsg()
{
	timerfd_settime(mSyncTimer,0,&ts,0);
	if(devEvCb)
		devEvCb->onSyncMsg();
}

void Device::onFirstSetupCmd(const Uuid &uuid,const char *name)
{
	if(devEvCb)
		devEvCb->onFirstSetupCmd(uuid,name);
}

void Device::checkTimer()
{
	uint64_t t;
	if(read(mSyncTimer,&t,sizeof(t))>0)
	{
		if(sockCb)
			sockCb->onSyncTimeout();
		mSock.disconnect();
	}
}
