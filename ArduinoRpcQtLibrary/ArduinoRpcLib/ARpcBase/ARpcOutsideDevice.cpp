#include "ARpcOutsideDevice.h"
#include "ARpcBase/ARpcSyncCall.h"
#include <QTimer>
#include <QDebug>
#include <QEventLoop>

ARpcOutsideDevice::ARpcOutsideDevice(QIODevice *d,QObject *parent)
	:QObject(parent)
	,streamParser(&msgParser)
{
	dev=d;
	connect(&streamParser,&ARpcStreamParser::processMessage,this,&ARpcOutsideDevice::rawMessage);
	if(dev)
	{
		connect(dev,&QIODevice::aboutToClose,this,&ARpcOutsideDevice::disconnected);
		connect(dev,&QIODevice::destroyed,this,&ARpcOutsideDevice::onDeviceDestroyed);
		connect(dev,&QIODevice::readyRead,this,&ARpcOutsideDevice::onDataReady);
	}
}

bool ARpcOutsideDevice::writeMsg(const QString &msg)
{
	return writeMsg(ARpcMessage(msg));
}

bool ARpcOutsideDevice::writeMsg(const QString &msg,const QStringList &args)
{
	return writeMsg(ARpcMessage(msg,args));
}

bool ARpcOutsideDevice::writeMsg(const ARpcMessage &m)
{
	if(!dev)return false;
	return dev->write((msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8());
}

void ARpcOutsideDevice::onDeviceDestroyed()
{
	dev=0;
}

void ARpcOutsideDevice::onDataReady()
{
	QByteArray data=dev->readAll();
	streamParser.pushData(QString::fromUtf8(data));
}
