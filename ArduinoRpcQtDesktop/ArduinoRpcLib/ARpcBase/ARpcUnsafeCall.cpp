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

#include "ARpcUnsafeCall.h"
#include "ARpcBase/ARpcRealDevice.h"
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

ARpcUnsafeCall::ARpcUnsafeCall(QObject *parent)
	:QObject(parent)
{
}

bool ARpcUnsafeCall::call(ARpcRealDevice *dev,const QByteArray &func,const QByteArrayList &args,QByteArrayList &retVal)
{
	if(!dev->isConnected())return false;
	QEventLoop loop;
	bool ok=false;
	auto conn1=connect(dev,&ARpcDevice::rawMessage,this,[&loop,this,&ok,&retVal](const ARpcMessage &m){
		if(m.title==ARpcConfig::funcAnswerOkMsg)
		{
			ok=true;
			retVal=m.args;
			loop.quit();
		}
		else if(m.title==ARpcConfig::funcAnswerErrMsg)
		{
			loop.quit();
			retVal=m.args;
			loop.quit();
		}
	},Qt::QueuedConnection);
	connect(this,&ARpcUnsafeCall::abortInternal,&loop,&QEventLoop::quit);
	connect(dev,&ARpcDevice::disconnected,&loop,&QEventLoop::quit);
	dev->writeMsg(ARpcMessage(ARpcConfig::funcCallMsg,QByteArrayList()<<func<<args));
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	disconnect(conn1);
	return ok;
}

bool ARpcUnsafeCall::call(ARpcRealDevice *dev,const QByteArray &func,QByteArrayList &retVal)
{
	return call(dev,func,QByteArrayList(),retVal);
}

void ARpcUnsafeCall::abort()
{
	emit abortInternal();
}
