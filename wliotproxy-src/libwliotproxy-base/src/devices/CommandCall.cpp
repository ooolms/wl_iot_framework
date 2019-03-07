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

#include "wliot/devices/CommandCall.h"
#include "wliot/devices/RealDevice.h"
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

CommandCall::CommandCall(RealDevice *d,const QByteArray &func,QObject *parent)
	:QObject(parent)
{
	dev=d;
	state=WAIT;
	ok=false;
	mFunc=func;
	mUseCallMsg=true;
	callId=QByteArray::number(qrand());
	timer.setInterval(WLIOTConfig::synccCallWaitTime);
	timer.setSingleShot(true);
	connect(&timer,&QTimer::timeout,this,&CommandCall::onTimeout);
	connect(dev,&RealDevice::disconnected,this,&CommandCall::onDeviceDisconnected);
	connect(dev,&RealDevice::destroyed,this,&CommandCall::onDeviceDisconnected);
	connect(dev,&RealDevice::newMessageFromDevice,this,&CommandCall::onNewMessage);
	connect(dev,&RealDevice::deviceWasReset,this,&CommandCall::onDeviceReset);
}

void CommandCall::setArgs(const QByteArrayList &args)
{
	if(state!=WAIT)return;
	mArgs=args;
}

void CommandCall::setUseCallMsg(bool u)
{
	if(state!=WAIT)return;
	mUseCallMsg=u;
	if(mUseCallMsg)
		callId=QByteArray::number(qrand());
	else callId.clear();
}

void CommandCall::setTimeout(int msec)
{
	if(state!=WAIT)return;
	timer.setInterval(msec);
}

const QByteArrayList& CommandCall::returnValue()
{
	return retVal;
}

bool CommandCall::call()
{
	if(state!=WAIT)return false;
	if(!dev||!dev->isConnected())
	{
		retVal.append("device disconnected");
		return false;
	}
	ok=false;
	state=EXEC;
	timer.start();
	if(mUseCallMsg)
		dev->writeMsgToDevice(Message(WLIOTConfig::funcCallMsg,QByteArrayList()<<callId<<mFunc<<mArgs));
	else dev->writeMsgToDevice(Message(mFunc,mArgs));
	if(state==EXEC)loop.exec(QEventLoop::ExcludeUserInputEvents);
	timer.stop();
	state=DONE;
	return ok;
}

void CommandCall::abort()
{
	retVal.append("aborted");
	loop.quit();
}

void CommandCall::onNewMessage(const Message &m)
{
	if(state!=EXEC)return;
	if(m.title==WLIOTConfig::funcAnswerOkMsg)
	{
		if(!callId.isEmpty()&&(m.args.isEmpty()||callId!=m.args[0]))return;
		ok=true;
		state=DONE;
		retVal=m.args;
		if(!callId.isEmpty())
			retVal.removeAt(0);
		loop.quit();
	}
	else if(m.title==WLIOTConfig::funcAnswerErrMsg)
	{
		if(!callId.isEmpty()&&(m.args.isEmpty()||callId!=m.args[0]))return;
		state=DONE;
		retVal=m.args;
		if(!callId.isEmpty())
			retVal.removeAt(0);
		loop.quit();
	}
	else if(m.title==WLIOTConfig::funcSynccMsg)
	{
		if(!callId.isEmpty()&&(m.args.isEmpty()||callId!=m.args[0]))return;
		timer.stop();
		timer.start();
	}
}

void CommandCall::onTimeout()
{
	if(state!=EXEC)return;
	retVal.append("timeout");
	loop.quit();
}

void CommandCall::onDeviceDisconnected()
{
	if(state!=EXEC)return;
	retVal.append("device disconnected");
	loop.quit();
}

void CommandCall::onDeviceDestroyed()
{
	dev=0;
	if(state!=EXEC)return;
	retVal.append("device disconnected");
	loop.quit();
}

void CommandCall::onDeviceReset()
{
	if(state!=EXEC)return;
	if(mUseCallMsg)
		dev->writeMsgToDevice(Message(WLIOTConfig::funcCallMsg,QByteArrayList()<<callId<<mFunc<<mArgs));
	else dev->writeMsgToDevice(Message(mFunc,mArgs));
}
