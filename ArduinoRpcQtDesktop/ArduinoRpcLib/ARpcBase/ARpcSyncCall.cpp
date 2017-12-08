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
	{
		retVal.append("device disconnected");
		return false;
	}
	QTimer t(this);
	t.setInterval(ARpcConfig::syncCallWaitTime);
	t.setSingleShot(true);
	QEventLoop loop;
	bool ok=false;
	auto conn1=connect(dev,&ARpcDevice::rawMessage,[&t,&loop,this,&ok,&retVal](const ARpcMessage &m)
	{
		if(m.title==ARpcConfig::funcAnswerOkMsg)
		{
			ok=true;
			retVal=m.args;
			loop.quit();
		}
		else if(m.title==ARpcConfig::funcAnswerErrMsg)
		{
			retVal=m.args;
			loop.quit();
		}
		else if(m.title==ARpcConfig::funcSyncMsg)
		{
			t.stop();
			t.start();
		}
	});
	auto conn2=connect(&t,&QTimer::timeout,[&loop,&retVal]
	{
		retVal.append("timeout");
		loop.quit();
	});
	auto conn3=connect(this,&ARpcSyncCall::abortInternal,[&loop,&retVal]
	{
		retVal.append("aborted");
		loop.quit();
	});
	auto conn4=connect(dev,&ARpcDevice::disconnected,[&loop,&retVal]
	{
		retVal.append("device disconnected");
		loop.quit();
	});
	t.start();
	dev->writeMsg(ARpcMessage(ARpcConfig::funcCallMsg,QStringList(func)<<args));
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	disconnect(conn1);
	disconnect(conn2);
	disconnect(conn3);
	disconnect(conn4);
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
