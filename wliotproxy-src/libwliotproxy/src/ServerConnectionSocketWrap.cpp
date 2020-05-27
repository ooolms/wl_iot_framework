/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ServerConnectionSocketWrap.h"
#include "wliot/client/ServerConnection.h"

const QString localServerName=QString("wliotproxyd");

using namespace WLIOT;
using namespace WLIOTClient;

ServerConnectionSocketWrap::ServerConnectionSocketWrap(ServerConnection *conn)
{
	netSock=0;
	connection=conn;
	connect(this,SIGNAL(newData(QByteArray)),connection,SLOT(onNewData(QByteArray)),Qt::QueuedConnection);
	connect(this,&ServerConnectionSocketWrap::connectionError,
		connection,&ServerConnection::connectionError,Qt::QueuedConnection);
}

void ServerConnectionSocketWrap::startConnectLocal()
{
	localSock=new QLocalSocket(this);
	connect(localSock,&QLocalSocket::connected,connection,
		&ServerConnection::onLocalSocketConnected,Qt::QueuedConnection);
	connect(localSock,&QLocalSocket::disconnected,
		connection,&ServerConnection::onDevDisconnected,Qt::QueuedConnection);
	connect(localSock,static_cast<void (QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
		this,&ServerConnectionSocketWrap::onLocalSocketError,Qt::DirectConnection);
	connect(localSock,SIGNAL(readyRead()),this,
		SLOT(onLocalReadyRead()),Qt::DirectConnection);
	localSock->connectToServer(localServerName);
}

void ServerConnectionSocketWrap::startConnectNet()
{
	netSock=new QSslSocket(this);
	netSock->setProxy(connection->proxy);
	netSock->setPeerVerifyMode(QSslSocket::VerifyNone);
	netSock->setSocketOption(QAbstractSocket::LowDelayOption,1);
	connect(netSock,&QSslSocket::encrypted,connection,&ServerConnection::onNetDeviceConnected,Qt::QueuedConnection);
	connect(netSock,&QSslSocket::disconnected,connection,&ServerConnection::onDevDisconnected,Qt::QueuedConnection);
	connect(netSock,static_cast<void(QSslSocket::*)(QAbstractSocket::SocketError)>(&QSslSocket::error),
		this,&ServerConnectionSocketWrap::onNetError,Qt::DirectConnection);
	connect(netSock,static_cast<void(QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&ServerConnectionSocketWrap::onSslError,Qt::DirectConnection);
	connect(netSock,SIGNAL(readyRead()),this,SLOT(onNetReadyRead()),Qt::DirectConnection);
	netSock->connectToHostEncrypted(connection->mHost,connection->mPort);
}

void ServerConnectionSocketWrap::disconnectFromServer()
{
	if(!connection->noDebug)
		qDebug()<<"ServerConnectionSocketWrap::disconnectFromServer";
	if(connection->netConn)
		netSock->disconnectFromHost();
	else localSock->disconnectFromServer();
}

void ServerConnectionSocketWrap::writeData(QByteArray data)
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

void ServerConnectionSocketWrap::onLocalSocketError()
{
	if(!connection->noDebug)qDebug()<<"iot server local socket error: "<<localSock->errorString();
	emit connectionError();
	if(localSock->isOpen())
		localSock->disconnectFromServer();
}

void ServerConnectionSocketWrap::onNetError()
{
	if(!connection->noDebug)qDebug()<<"iot server net socket error: "<<netSock->errorString();
	emit connectionError();
	if(netSock->isOpen())
		netSock->disconnectFromHost();
}

void ServerConnectionSocketWrap::onSslError()
{
	if(!connection->noDebug)
		qDebug()<<"iot server net ssl error: "<<netSock->sslErrors();
//	netSock->ignoreSslErrors(netSock->sslErrors());
	/*delete dev;
	netSock->deleteLater();
	netSock=0;
	dev=0;*/
}

void ServerConnectionSocketWrap::onLocalReadyRead()
{
	QByteArray data=localSock->readAll();
	if(!data.isEmpty())
		emit newData(data);
}

void ServerConnectionSocketWrap::onNetReadyRead()
{
	QByteArray data=netSock->readAll();
	if(!data.isEmpty())
		emit newData(data);
}
