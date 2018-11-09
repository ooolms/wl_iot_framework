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

#include "ARpcSyncCall.h"
#include "ARpcBase/ARpcRealDevice.h"
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

ARpcSyncCall::ARpcSyncCall(ARpcRealDevice *d,const QByteArray &func,QObject *parent)
	:QObject(parent)
{
	dev=d;
	state=WAIT;
	ok=false;
	mFunc=func;
	mUseCallMsg=true;
	callId=QByteArray::number(qrand());
	timer.setInterval(ARpcConfig::synccCallWaitTime);
	timer.setSingleShot(true);
	connect(&timer,&QTimer::timeout,this,&ARpcSyncCall::onTimeout);
	connect(dev,&ARpcDevice::disconnected,this,&ARpcSyncCall::onDeviceDisconnected);
	connect(dev,&ARpcDevice::destroyed,this,&ARpcSyncCall::onDeviceDisconnected);
	connect(dev,&ARpcDevice::newMessage,this,&ARpcSyncCall::onNewMessage);
	connect(dev,&ARpcDevice::streamWasReset,this,&ARpcSyncCall::onStreamReset);
}

void ARpcSyncCall::setArgs(const QByteArrayList &args)
{
	if(state!=WAIT)return;
	mArgs=args;
}

void ARpcSyncCall::setUseCallMsg(bool u)
{
	if(state!=WAIT)return;
	mUseCallMsg=u;
	if(mUseCallMsg)
		callId=QByteArray::number(qrand());
	else callId.clear();
}

void ARpcSyncCall::setTimeout(int msec)
{
	if(state!=WAIT)return;
	timer.setInterval(msec);
}

const QByteArrayList& ARpcSyncCall::returnValue()
{
	return retVal;
}

bool ARpcSyncCall::call()
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
		dev->writeMsg(ARpcMessage(ARpcConfig::funcCallMsg,QByteArrayList()<<callId<<mFunc<<mArgs));
	else dev->writeMsg(ARpcMessage(mFunc,mArgs));
	if(state==EXEC)loop.exec(QEventLoop::ExcludeUserInputEvents);
	timer.stop();
	state=DONE;
	return ok;
}

void ARpcSyncCall::abort()
{
	retVal.append("aborted");
	loop.quit();
}

void ARpcSyncCall::onNewMessage(const ARpcMessage &m)
{
	if(state!=EXEC)return;
	if(m.title==ARpcConfig::funcAnswerOkMsg)
	{
		if(!callId.isEmpty()&&(m.args.isEmpty()||callId!=m.args[0]))return;
		ok=true;
		state=DONE;
		retVal=m.args;
		if(!callId.isEmpty())
			retVal.removeAt(0);
		loop.quit();
	}
	else if(m.title==ARpcConfig::funcAnswerErrMsg)
	{
		if(!callId.isEmpty()&&(m.args.isEmpty()||callId!=m.args[0]))return;
		state=DONE;
		retVal=m.args;
		if(!callId.isEmpty())
			retVal.removeAt(0);
		loop.quit();
	}
	else if(m.title==ARpcConfig::funcSynccMsg)
	{
		if(!callId.isEmpty()&&(m.args.isEmpty()||callId!=m.args[0]))return;
		timer.stop();
		timer.start();
	}
}

void ARpcSyncCall::onTimeout()
{
	if(state!=EXEC)return;
	retVal.append("timeout");
	loop.quit();
}

void ARpcSyncCall::onDeviceDisconnected()
{
	if(state!=EXEC)return;
	retVal.append("device disconnected");
	loop.quit();
}

void ARpcSyncCall::onDeviceDestroyed()
{
	dev=0;
	if(state!=EXEC)return;
	retVal.append("device disconnected");
	loop.quit();
}

void ARpcSyncCall::onStreamReset()
{
	if(state!=EXEC)return;
	if(mUseCallMsg)
		dev->writeMsg(ARpcMessage(ARpcConfig::funcCallMsg,QByteArrayList()<<callId<<mFunc<<mArgs));
	else dev->writeMsg(ARpcMessage(mFunc,mArgs));
}
