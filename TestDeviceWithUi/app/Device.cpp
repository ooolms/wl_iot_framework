#include "Device.h"
#include <QNetworkDatagram>
#include <QDebug>

static const ARpcUuid devId("{bfb3e09b-7d6b-4c30-a48d-60e86d5f82f6}");
static const QByteArray devName="test_vdev";

Device::Device(QObject *parent)
	:QObject(parent)
	,srvReady(200,this)
	,dev(400,this,&devId,devName)
{
	bCastCli=new QUdpSocket(this);
	bCastCli->bind(QHostAddress::AnyIPv4,4081);
	client=new QTcpSocket(this);
	connect(bCastCli,&QUdpSocket::readyRead,this,&Device::onBCastCliReadyRead);
	connect(client,&QTcpSocket::readyRead,this,&Device::onSocketReadyRead);
}

void Device::processSrvReadyMsg(const ARpcUuid &serverId,const char *serverName)
{
	char idStr[39];
	serverId.toString(idStr);
	qDebug()<<"Server detected: "<<idStr<<", "<<serverName;
}

void Device::writeData(const char *data,unsigned long sz)
{
	client->write(data,(qint64)sz);
}

void Device::writeStr(const char *str)
{
	client->write(str);
}

void Device::onBCastCliReadyRead()
{
	while(bCastCli->hasPendingDatagrams())
	{
		QNetworkDatagram dg=bCastCli->receiveDatagram();
		bCastSerderAddr=dg.senderAddress();
		QByteArray d=dg.data();
		srvReady.putData(d.constData(),(unsigned long)d.size());
	}
}

void Device::onSocketReadyRead()
{
	QByteArray d=client->readAll();
	dev.putData(d.constData(),(quint32)d.size());
}
