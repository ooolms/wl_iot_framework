#include "ARpcSyncCall.h"
#include "ARpcDevice.h"
#include <QTimer>
#include <QEventLoop>

const int ARpcSyncCall::defaultTimeout=2000;

ARpcSyncCall::ARpcSyncCall(QObject *parent)
	:QObject(parent)
{
	okMessage="ok";
	errMessage="err";
	syncMessage="sync";
}

ARpcSyncCall::ARpcSyncCall(const QString &okMsg,const QString &errMsg,QObject *parent)
	:QObject(parent)
{
	okMessage=okMsg;
	errMessage=errMsg;
	syncMessage="sync";
}

ARpcSyncCall::ARpcSyncCall(const QString &okMsg,const QString &errMsg,const QString &syncMsg,QObject *parent)
	:QObject(parent)
{
	okMessage=okMsg;
	errMessage=errMsg;
	syncMessage=syncMsg;
}

bool ARpcSyncCall::call(const ARpcMessage &callMsg,ARpcDevice *dev,QStringList &retVal)
{
	QTimer t(this);
	t.setInterval(2000);
	t.setSingleShot(true);
	QEventLoop loop;
	bool ok=false;
	auto conn1=connect(dev,&ARpcDevice::rawMessage,this,[&t,&loop,this,&ok,&retVal](const ARpcMessage &m){
		if(m.title==okMessage)
		{
			ok=true;
			retVal=m.args;
			loop.quit();
		}
		else if(m.title==errMessage)
		{
			loop.quit();
			retVal=m.args;
			loop.quit();
		}
		else if(m.title==syncMessage)
		{
			t.stop();
			t.start();
		}
	});
	connect(&t,&QTimer::timeout,this,[&t,&loop,&conn1](){
		loop.quit();
		disconnect(conn1);
	});
	t.start();
	dev->writeMsg(callMsg);
	loop.exec();
	return ok;
}
