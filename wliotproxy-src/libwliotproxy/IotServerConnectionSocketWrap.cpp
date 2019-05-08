#include "IotServerConnectionSocketWrap.h"
#include "IotServerConnection.h"

const QString localServerName=QString("wliotproxyd");

IotServerConnectionSocketWrap::IotServerConnectionSocketWrap(IotServerConnection *conn)
{
	netSock=0;
	connection=conn;
	connect(this,&IotServerConnectionSocketWrap::newData,
		connection,&IotServerConnection::onNewData,Qt::QueuedConnection);
	connect(this,&IotServerConnectionSocketWrap::connectionError,
		connection,&IotServerConnection::connectionError,Qt::QueuedConnection);
}

void IotServerConnectionSocketWrap::startConnectLocal()
{
	localSock=new QLocalSocket(this);
	connect(localSock,&QLocalSocket::connected,connection,
		&IotServerConnection::onLocalSocketConnected,Qt::QueuedConnection);
	connect(localSock,&QLocalSocket::disconnected,
		connection,&IotServerConnection::onDevDisconnected,Qt::QueuedConnection);
	connect(localSock,static_cast<void (QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
		this,&IotServerConnectionSocketWrap::onLocalSocketError,Qt::DirectConnection);
	connect(localSock,&QSslSocket::readyRead,this,
		&IotServerConnectionSocketWrap::onLocalReadyRead,Qt::DirectConnection);
	localSock->connectToServer(localServerName);
}

void IotServerConnectionSocketWrap::startConnectNet()
{
	netSock=new QSslSocket(this);
	netSock->setProxy(connection->proxy);
	netSock->setPeerVerifyMode(QSslSocket::VerifyNone);
	netSock->setSocketOption(QAbstractSocket::LowDelayOption,1);
	connect(netSock,&QSslSocket::encrypted,connection,&IotServerConnection::onNetDeviceConnected,Qt::QueuedConnection);
	connect(netSock,&QSslSocket::disconnected,connection,&IotServerConnection::onDevDisconnected,Qt::QueuedConnection);
	connect(netSock,static_cast<void(QSslSocket::*)(QAbstractSocket::SocketError)>(&QSslSocket::error),
		this,&IotServerConnectionSocketWrap::onNetError,Qt::DirectConnection);
	connect(netSock,static_cast<void(QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&IotServerConnectionSocketWrap::onSslError,Qt::DirectConnection);
	connect(netSock,&QSslSocket::readyRead,this,&IotServerConnectionSocketWrap::onNetReadyRead,Qt::DirectConnection);
	netSock->connectToHostEncrypted(connection->mHost,connection->mPort);
}

void IotServerConnectionSocketWrap::disconnectFromServer()
{
	qDebug()<<"IotServerConnectionSocketWrap::disconnectFromServer";
	if(connection->netConn)
		netSock->disconnectFromHost();
	else localSock->disconnectFromServer();
}

void IotServerConnectionSocketWrap::writeData(QByteArray data)
{
	if(connection->netConn)
	{
		netSock->write(data);
		netSock->flush();
	}
	else
	{
		localSock->write(data);
		localSock->flush();
	}
}

void IotServerConnectionSocketWrap::onLocalSocketError()
{
	qDebug()<<"iot server local socket error: "<<localSock->errorString();
	emit connectionError();
	if(localSock->isOpen())
		localSock->disconnectFromServer();
}

void IotServerConnectionSocketWrap::onNetError()
{
	qDebug()<<"iot server net socket error: "<<netSock->errorString();
	emit connectionError();
	if(netSock->isOpen())
		netSock->disconnectFromHost();
}

void IotServerConnectionSocketWrap::onSslError()
{
	qDebug()<<"iot server net ssl error: "<<netSock->sslErrors();
//	netSock->ignoreSslErrors(netSock->sslErrors());
	/*delete dev;
	netSock->deleteLater();
	netSock=0;
	dev=0;*/
}

void IotServerConnectionSocketWrap::onLocalReadyRead()
{
	QByteArray data=localSock->readAll();
	if(!data.isEmpty())
		emit newData(data);
}

void IotServerConnectionSocketWrap::onNetReadyRead()
{
	QByteArray data=netSock->readAll();
	if(!data.isEmpty())
		emit newData(data);
}
