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

bool ARpcSyncCall::call(const ARpcMessage &callMsg,ARpcDevice *dev,QStringList &retVal)
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
	connect(&t,&QTimer::timeout,this,[&t,&loop,&conn1](){
		loop.quit();
	});
	t.start();
	dev->writeMsg(callMsg);
	loop.exec();
	disconnect(conn1);
	return ok;
}
