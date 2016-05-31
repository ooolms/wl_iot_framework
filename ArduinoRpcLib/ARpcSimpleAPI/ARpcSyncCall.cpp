#include "ARpcSyncCall.h"
#include "ARpcBase/ARpcDevice.h"
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

const int ARpcSyncCall::defaultTimeout=2000;

ARpcSyncCall::ARpcSyncCall(ARpcConfig &cfg,QObject *parent)
	:QObject(parent)
	,config(cfg)
{
}

bool ARpcSyncCall::call(ARpcDevice *dev,const ARpcMessage &callMsg,QStringList &retVal)
{
	if(!dev->isConnected())return false;
	QTimer t(this);
	t.setInterval(config.syncCallWaitTime);
	t.setSingleShot(true);
	QEventLoop loop;
	bool ok=false;
	auto conn1=connect(dev,&ARpcDevice::rawMessage,this,[&t,&loop,this,&ok,&retVal](const ARpcMessage &m){
		if(m.title==config.funcCallOkMsgTitle)
		{
			ok=true;
			retVal=m.args;
			loop.quit();
		}
		else if(m.title==config.funcCallErrMsgTitle)
		{
			loop.quit();
			retVal=m.args;
			loop.quit();
		}
		else if(m.title==config.funcCallSyncMsgTitle)
		{
			t.stop();
			t.start();
		}
//		else if(m.title==config.infoMsgTitle)
//		{
//			qDebug()<<"MSG: "<<m.title<<" ARGS: "<<m.args;
//		}
	});
	connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(this,&ARpcSyncCall::abortInternal,&loop,&QEventLoop::quit);
	connect(dev,&ARpcDevice::disconnected,&loop,&QEventLoop::quit);
	t.start();
	dev->writeMsg(callMsg);
	loop.exec();
	disconnect(conn1);
	return ok;
}

void ARpcSyncCall::abort()
{
	emit abortInternal();
}
