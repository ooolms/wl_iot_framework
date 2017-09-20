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

ARpcSyncCall::ARpcSyncCall(QObject *parent)
	:QObject(parent)
{
}

bool ARpcSyncCall::call(ARpcRealDevice *dev,const QString &func,const QStringList &args,QStringList &retVal)
{
	if(!dev->isConnected())
		return false;
	QTimer t(this);
	t.setInterval(ARpcConfig::syncCallWaitTime);
	t.setSingleShot(true);
	QEventLoop loop;
	bool ok=false;
	auto conn1=connect(dev,&ARpcDevice::rawMessage,this,[&t,&loop,this,&ok,&retVal](const ARpcMessage &m)
	{
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
		else if(m.title==ARpcConfig::funcSyncMsg)
		{
			t.stop();
			t.start();
		}
	},Qt::QueuedConnection);
	connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(this,&ARpcSyncCall::abortInternal,&loop,&QEventLoop::quit);
	connect(dev,&ARpcDevice::disconnected,&loop,&QEventLoop::quit);
	t.start();
	dev->writeMsg(ARpcMessage(ARpcConfig::funcCallMsg,QStringList(func)<<args));
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	disconnect(conn1);
	return ok;
}

bool ARpcSyncCall::call(ARpcRealDevice *dev,const QString &func,QStringList &retVal)
{
	return call(dev,func,QStringList(),retVal);
}

void ARpcSyncCall::abort()
{
	emit abortInternal();
}
