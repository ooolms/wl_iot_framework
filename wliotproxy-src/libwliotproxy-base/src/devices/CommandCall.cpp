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
	connect(&timer,&QTimer::timeout,this,&CommandCall::onTimeout,Qt::DirectConnection);
	connect(dev,&RealDevice::disconnected,this,&CommandCall::onDeviceDisconnected,Qt::DirectConnection);
	connect(dev,&RealDevice::destroyed,this,&CommandCall::onDeviceDisconnected,Qt::DirectConnection);
	connect(dev,&RealDevice::deviceWasReset,this,&CommandCall::onDeviceReset,Qt::DirectConnection);
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

void CommandCall::setupTimer(int msec)
{
	if(state!=WAIT)return;
	timer.setInterval(msec);
	timer.setSingleShot(true);
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
		state=DONE;
		return false;
	}
	auto conn=connect(dev,&RealDevice::newMessageFromDevice,this,&CommandCall::onNewMessage,Qt::DirectConnection);
	state=EXEC;
	if(mUseCallMsg)
		dev->writeMsgToDevice(Message(WLIOTProtocolDefs::funcCallMsg,QByteArrayList()<<callId<<mFunc<<mArgs));
	else dev->writeMsgToDevice(Message(mFunc,mArgs));
	if(state==EXEC)
	{
		if(timer.interval()!=0)
			timer.start();
		loop.exec();
	}
	timer.stop();
	state=DONE;
	disconnect(conn);
	return ok;
}

void CommandCall::abort()
{
	retVal.append("aborted");
	loop.quit();
}

void CommandCall::reset()
{
	if(state!=DONE)return;
	state=WAIT;
	retVal.clear();
	ok=false;
}

void CommandCall::onNewMessage(const Message &m)
{
	if(m.title==WLIOTProtocolDefs::funcAnswerOkMsg)
	{
		if(!callId.isEmpty()&&(m.args.isEmpty()||callId!=m.args[0]))return;
		loop.quit();
		if(state==DONE)return;
		ok=true;
		state=DONE;
		retVal=m.args;
		if(!callId.isEmpty())
			retVal.removeAt(0);
	}
	else if(m.title==WLIOTProtocolDefs::funcAnswerErrMsg)
	{
		if(!callId.isEmpty()&&(m.args.isEmpty()||callId!=m.args[0]))return;
		loop.quit();
		if(state==DONE)return;
		state=DONE;
		retVal=m.args;
		if(!callId.isEmpty())
			retVal.removeAt(0);
	}
}

void CommandCall::onTimeout()
{
	onError("timeout");
}

void CommandCall::onDeviceDisconnected()
{
	onError("device disconnected");
}

void CommandCall::onDeviceDestroyed()
{
	dev=0;
	onError("device disconnected");
}

void CommandCall::onDeviceReset()
{
	onError("device was reset");
}

void CommandCall::onError(const QByteArray &msg)
{
	if(state!=EXEC)return;
	retVal=QByteArrayList()<<msg;
	ok=false;
	state=DONE;
	timer.stop();
	loop.quit();
}
