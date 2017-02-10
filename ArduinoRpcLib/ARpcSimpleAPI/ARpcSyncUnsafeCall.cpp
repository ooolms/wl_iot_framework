#include "ARpcSyncUnsafeCall.h"
#include "ARpcBase/ARpcDevice.h"
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

ARpcSyncUnsafeCall::ARpcSyncUnsafeCall(QObject *parent)
	:QObject(parent)
{
}

bool ARpcSyncUnsafeCall::call(ARpcDevice *dev,const QString &func,const QStringList &args,QStringList &retVal)
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
	},Qt::DirectConnection);
	connect(this,&ARpcSyncUnsafeCall::abortInternal,&loop,&QEventLoop::quit);
	connect(dev,&ARpcDevice::disconnected,&loop,&QEventLoop::quit);
	dev->writeMsg(ARpcMessage(ARpcConfig::funcCallMsg,QStringList(func)<<args));
	loop.exec();
	disconnect(conn1);
	return ok;
}

bool ARpcSyncUnsafeCall::call(ARpcDevice *dev,const QString &func,QStringList &retVal)
{
	return call(dev,func,QStringList(),retVal);
}

void ARpcSyncUnsafeCall::abort()
{
	emit abortInternal();
}
