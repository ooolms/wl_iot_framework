#include "ARpcSyncUnsafeCall.h"
#include "ARpcBase/ARpcDevice.h"
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

ARpcSyncUnsafeCall::ARpcSyncUnsafeCall(ARpcConfig &cfg,QObject *parent)
	:QObject(parent)
	,config(cfg)
{
}

bool ARpcSyncUnsafeCall::call(const ARpcMessage &callMsg,ARpcDevice *dev,QStringList &retVal)
{
	if(!dev->isConnected())return false;
	QEventLoop loop;
	bool ok=false;
	auto conn1=connect(dev,&ARpcDevice::rawMessage,this,[&loop,this,&ok,&retVal](const ARpcMessage &m){
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
//		else if(m.title==config.infoMsgTitle)
//		{
//			qDebug()<<"MSG: "<<m.title<<" ARGS: "<<m.args;
//		}
	});
	dev->writeMsg(callMsg);
	loop.exec();
	disconnect(conn1);
	return ok;
}
