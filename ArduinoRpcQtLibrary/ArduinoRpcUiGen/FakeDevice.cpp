#include "FakeDevice.h"
#include <QDebug>
#include <QThread>

FakeDevice::FakeDevice(QObject *parent)
	:ARpcDevice(parent)
{
	devId=QUuid::createUuid();
}

bool FakeDevice::writeMsg(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::identifyMsg)
	{
		emit logMsg("Identify request");
		QThread::usleep(100);
		emit rawMessage(ARpcMessage(ARpcConfig::deviceInfoMsg,QStringList()<<devId.toString()<<"Test device"));
	}
	else if(m.title==ARpcConfig::funcCallMsg&&!m.args.isEmpty())
	{
		QStringList args=m.args;
		args.removeAt(0);
		emit logMsg("Function called: "+m.args[0]+"; args: "+args.join("|"));
		QThread::usleep(100);
		emit rawMessage(ARpcMessage("ok"));
	}
	return true;
}

bool FakeDevice::isConnected()
{
	return true;
}
