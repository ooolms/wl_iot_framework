#include "ARpcSyncCall.h"
#include "ARpcBase/ARpcDevice.h"
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

ARpcSyncCall::ARpcSyncCall(QObject *parent)
	:QObject(parent)
{
}

bool ARpcSyncCall::call(ARpcDevice *dev,const QString &func,const QStringList &args,QStringList &retVal)
{
	if(!dev->isConnected())return false;
	QTimer t(this);
	t.setInterval(ARpcConfig::syncCallWaitTime);
	t.setSingleShot(true);
	QEventLoop loop;
	bool ok=false;
	auto conn1=connect(dev,&ARpcDevice::rawMessage,this,[&t,&loop,this,&ok,&retVal](const ARpcMessage &m){
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

bool ARpcSyncCall::call(ARpcDevice *dev,const QString &func,QStringList &retVal)
{
	return call(dev,func,QStringList(),retVal);
}

void ARpcSyncCall::abort()
{
	emit abortInternal();
}
