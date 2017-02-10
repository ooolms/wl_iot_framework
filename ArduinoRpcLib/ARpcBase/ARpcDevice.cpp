#include "ARpcDevice.h"
#include <QTimer>
#include <QEventLoop>

ARpcDevice::ARpcDevice(QObject *parent)
	:QObject(parent)
	,streamParser(&msgParser)
{
	connect(&streamParser,&ARpcStreamParser::processMessage,this,&ARpcDevice::rawMessage);
}

bool ARpcDevice::writeMsg(const QString &msg)
{
	return writeMsg(ARpcMessage(msg));
}

bool ARpcDevice::writeMsg(const QString &msg,const QStringList &args)
{
	return writeMsg(ARpcMessage(msg,args));
}

bool ARpcDevice::identify()
{
	if(!isConnected())return false;
	QTimer t(this);
	t.setInterval(ARpcConfig::identifyWaitTime);
	t.setSingleShot(true);
	QEventLoop loop;
	QStringList retVal;
	bool ok=false;
	auto conn1=connect(this,&ARpcDevice::rawMessage,this,[&t,&loop,this,&ok,&retVal](const ARpcMessage &m){
		if(m.title==ARpcConfig::deviceInfoMsg)
		{
			ok=true;
			retVal=m.args;
			loop.quit();
		}
	});
	connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(this,&ARpcDevice::disconnected,&loop,&QEventLoop::quit);
	t.start();
	writeMsg(ARpcMessage(ARpcConfig::identifyMsg));
	loop.exec();
	disconnect(conn1);
	if(!ok)return false;
	if(retVal.count()<2)return false;
	if(retVal[1].isEmpty())return false;
	uuid=QUuid(retVal[0]);
	deviceName=retVal[1];
	return !uuid.isNull();
}

bool ARpcDevice::isIdentified()
{
	return !uuid.isNull();
}

QString ARpcDevice::name()
{
	return deviceName;
}
