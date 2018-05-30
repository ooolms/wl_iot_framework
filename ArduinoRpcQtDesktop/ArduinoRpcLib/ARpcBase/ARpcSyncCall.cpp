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

ARpcSyncCall::ARpcSyncCall(ARpcRealDevice *d,QObject *parent)
	:QObject(parent)
{
	dev=d;
	done=true;
	ok=false;
	timer.setInterval(ARpcConfig::syncCallWaitTime);
	timer.setSingleShot(true);
	connect(&timer,&QTimer::timeout,this,&ARpcSyncCall::onTimeout);
	connect(dev,&ARpcDevice::disconnected,this,&ARpcSyncCall::onDeviceDisconnected);
	connect(dev,&ARpcDevice::destroyed,this,&ARpcSyncCall::onDeviceDisconnected);
	connect(dev,&ARpcDevice::rawMessage,this,&ARpcSyncCall::onRawMessage);
}

QByteArrayList ARpcSyncCall::returnValue()
{
	return retVal;
}

bool ARpcSyncCall::call(const QByteArray &func,bool useCallMsg)
{
	return call(func,QByteArrayList(),useCallMsg);
}

bool ARpcSyncCall::call(const QByteArray &func,const QByteArrayList &args,bool useCallMsg)
{
	if(!done)return false;
	if(!dev||!dev->isConnected())
	{
		retVal.append("device disconnected");
		return false;
	}
	ok=false;
	done=false;
	timer.start();
	if(useCallMsg)
		dev->writeMsg(ARpcMessage(ARpcConfig::funcCallMsg,QByteArrayList()<<func<<args));
	else dev->writeMsg(ARpcMessage(func,args));
	if(!done)loop.exec(QEventLoop::ExcludeUserInputEvents);
	timer.stop();
	done=true;
	return ok;
}

void ARpcSyncCall::abort()
{
	retVal.append("aborted");
	loop.quit();
}

void ARpcSyncCall::onRawMessage(const ARpcMessage &m)
{
	if(done)return;
	if(m.title==ARpcConfig::funcAnswerOkMsg)
	{
		ok=true;
		done=true;
		retVal=m.args;
		loop.quit();
	}
	else if(m.title==ARpcConfig::funcAnswerErrMsg)
	{
		done=true;
		retVal=m.args;
		loop.quit();
	}
	else if(m.title==ARpcConfig::funcSyncMsg)
	{
		timer.stop();
		timer.start();
	}
}

void ARpcSyncCall::onTimeout()
{
	retVal.append("timeout");
	loop.quit();
}

void ARpcSyncCall::onDeviceDisconnected()
{
	retVal.append("device disconnected");
	loop.quit();
}

void ARpcSyncCall::onDeviceDestroyed()
{
	retVal.append("device disconnected");
	loop.quit();
	dev=0;
}
