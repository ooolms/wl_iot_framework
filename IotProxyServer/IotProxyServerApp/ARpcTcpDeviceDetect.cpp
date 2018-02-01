#include "ARpcTcpDeviceDetect.h"
#include "ARpcBase/ARpcConfig.h"
#include <QDebug>
#include <QTcpSocket>
#include <QUuid>

ARpcTcpDeviceDetect::ARpcTcpDeviceDetect(QObject *parent)
	:QObject(parent)
{
	if(!srv.listen(QHostAddress::AnyIPv4,ARpcConfig::netDevicePort))
		qDebug()<<"Can't start listening for tcp devices";
	tmr.setSingleShot(false);
	connect(&srv,&ARpcTcpDeviceDetectServer::newClient,this,&ARpcTcpDeviceDetect::newClient);
	connect(&tmr,&QTimer::timeout,this,&ARpcTcpDeviceDetect::broadcastServerReadyMessage);
}

void ARpcTcpDeviceDetect::broadcastServerReadyMessage()
{
	bCastSock.writeDatagram(ARpcConfig::serverReadyMsg+ARpcConfig::msgDelim,
		QHostAddress::Broadcast,ARpcConfig::netDevicePort);
}

bool ARpcTcpDeviceDetect::isServerListening()
{
	return srv.isListening();
}

void ARpcTcpDeviceDetect::startRegularBroadcasting(int msecsDelay)
{
	tmr.start(msecsDelay);
}

void ARpcTcpDeviceDetect::stopRegularBroadcasting()
{
	tmr.stop();
}
