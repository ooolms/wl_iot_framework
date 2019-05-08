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

CommandCall::CommandCall(const QByteArray &cmd,QObject *parent)
	:QObject(parent)
{
	dev=0;
	state=WAIT;
	mOk=false;
	mCommand=cmd;
	mUseCallMsg=true;
	connect(&timer,&QTimer::timeout,this,&CommandCall::onTimeout,Qt::DirectConnection);
	connect(qApp,&QCoreApplication::aboutToQuit,[this]()
	{
		abort();
	});
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

const QByteArrayList& CommandCall::returnValue()
{
	return retVal;
}

bool CommandCall::ok()
{
	return mOk;
}

void CommandCall::run(RealDevice *d,const QByteArray &mCallId)
{
	dev=d;
	state=EXEC;
	if(timer.interval()!=0)
		timer.start();
	if(mUseCallMsg)
		dev->writeMsgToDevice(Message(WLIOTProtocolDefs::funcCallMsg,QByteArrayList()<<mCallId<<mCommand<<mArgs));
	else dev->writeMsgToDevice(Message(mCommand,mArgs));
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
	onErrorMsg("device was reset");
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
