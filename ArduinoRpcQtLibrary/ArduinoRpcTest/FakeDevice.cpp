#include "FakeDevice.h"
#include <QDebug>
#include <QThread>

FakeDevice::FakeDevice(QObject *parent)
	:ARpcDevice(parent)
{
	devId=QUuid::createUuid();
}

void FakeDevice::msgFromDevice(const ARpcMessage &m)
{
	emit rawMessage(m);
}

bool FakeDevice::writeMsg(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::identifyMsg)
	{
		qDebug()<<"Identify request";
		QThread::usleep(100);
		emit rawMessage(ARpcMessage(ARpcConfig::deviceInfoMsg,QStringList()<<devId.toString()<<"Test device"));
	}
	else if(m.title==ARpcConfig::funcCallMsg&&!m.args.isEmpty())
	{
		QStringList args=m.args;
		args.removeAt(0);
		qDebug()<<"Function called: "<<m.args[0]<<"; args: "<<args.join("|");
		QThread::usleep(100);
		emit rawMessage(ARpcMessage("ok"));
	}
	return true;
}

bool FakeDevice::isConnected()
{
	return true;
}
