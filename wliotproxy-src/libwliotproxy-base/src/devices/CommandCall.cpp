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
#include <QCoreApplication>

using namespace WLIOT;

CommandCall::CommandCall(const QByteArray &cmd,QObject *parent)
	:QObject(parent)
{
	state=WAIT;
	mOk=false;
	mCommand=cmd;
	mUseCallMsg=true;
	mRecallOnDevReset=false;
	dev=0;
	connect(&timer,&QTimer::timeout,this,&CommandCall::onTimeout,Qt::DirectConnection);
}

CommandCall* CommandCall::setArgs(const QByteArrayList &args)
{
	if(state!=EXEC)
		mArgs=args;
	return this;
}

CommandCall* CommandCall::setUseCallMsg(bool u)
{
	if(state!=EXEC)
		mUseCallMsg=u;
	return this;
}

CommandCall* CommandCall::setupTimer(int msec)
{
	if(state!=EXEC)
	{
		timer.setInterval(msec);
		timer.setSingleShot(true);
	}
	return this;
}

CommandCall* CommandCall::setRecallOnDevReset(bool en)
{
	if(state!=EXEC)
		mRecallOnDevReset=en;
	return this;
}

const QByteArrayList& CommandCall::returnValue()
{
	return retVal;
}

bool CommandCall::ok()
{
	return mOk;
}

void CommandCall::run(RealDevice *d,const QByteArray &callIdStr)
{
	state=EXEC;
	dev=d;
	mCallId=callIdStr;
	if(timer.interval()!=0)
		timer.start();
	bool msgOk=false;
	if(mUseCallMsg)
		msgOk=d->writeMsgToDevice(Message(WLIOTProtocolDefs::funcCallMsg,QByteArrayList()<<mCallId<<mCommand<<mArgs));
	else msgOk=d->writeMsgToDevice(Message(mCommand,mArgs));
	if(!msgOk)
	{
		state=DONE;
		mOk=false;
		retVal=QByteArrayList()<<"Error when sending \"call\" message to device";
	}
}

void CommandCall::onOkMsg(const QByteArrayList &args)
{
	if(state!=EXEC)return;
	retVal=args;
	mOk=true;
	state=DONE;
	timer.stop();
	emit done();
}

void CommandCall::onErrorMsg(const QByteArray &msg)
{
	onErrorMsg(QByteArrayList()<<msg);
}

void CommandCall::abort()
{
	onErrorMsg("aborted");
}

bool CommandCall::wait()
{
	if(state==DONE)
		return mOk;
	else if(state==WAIT)
		return false;
	while(state==EXEC)
		qApp->processEvents(QEventLoop::WaitForMoreEvents);
	return mOk;
}

bool CommandCall::isWorking()
{
	return state==EXEC;
}

QByteArray CommandCall::callId()
{
	return mCallId;
}

void CommandCall::onTimeout()
{
	onErrorMsg("timeout");
}

void CommandCall::onDeviceDisconnected()
{
	onErrorMsg("device disconnected");
}

void CommandCall::onDeviceDestroyed()
{
	dev=0;
	onErrorMsg("device disconnected");
}

void CommandCall::onDeviceReset()
{
	if(state!=EXEC)return;
	if(mRecallOnDevReset&&dev)
	{
		run(dev,mCallId);
//		if(timer.interval()!=0)
//			timer.start();
//		if(mUseCallMsg)
//			dev->writeMsgToDevice(Message(WLIOTProtocolDefs::funcCallMsg,QByteArrayList()<<mCallId<<mCommand<<mArgs));
//		else dev->writeMsgToDevice(Message(mCommand,mArgs));
	}
	else onErrorMsg("device was reset");
}

void CommandCall::onErrorMsg(const QByteArrayList &args)
{
	if(state!=EXEC)return;
	retVal=args;
	mOk=false;
	state=DONE;
	timer.stop();
	emit done();
}
