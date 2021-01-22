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
#include <QFile>
#include <QSocketNotifier>

#ifdef Q_OS_WIN
#else
#include <unistd.h>
#include <fcntl.h>
#endif

const QString localServerName=QString("wliotproxyd");

namespace WLIOTClient
{
	class StdInOut
	{
	public:
		StdInOut()
		{
#ifdef Q_OS_WIN
			ok=false;
			stdinNotif=0;
			//non-block read of stdin is not supported on Win
#else
			ok=stdinFile.open(STDIN_FILENO,QFile::ReadOnly|QIODevice::Unbuffered)&&
				stdoutFile.open(STDOUT_FILENO,QFile::WriteOnly|QIODevice::Unbuffered);
			int flags=fcntl(STDIN_FILENO,F_GETFL);
			flags|=O_NONBLOCK;
			fcntl(STDIN_FILENO,F_SETFL,flags);
			stdinNotif=new QSocketNotifier(STDIN_FILENO,QSocketNotifier::Read);
#endif
		}
		~StdInOut()
		{
			if(stdinNotif)
				delete stdinNotif;
		}

	public:
		QFile stdinFile;
		QFile stdoutFile;
		QSocketNotifier *stdinNotif;
		QMetaObject::Connection notifConn;
		bool ok;
	};
}

using namespace WLIOT;
using namespace WLIOTClient;

ServerConnectionSocketWrap::ServerConnectionSocketWrap(ServerConnection *conn)
{
	connType=conn->connType;
	netSock=0;
	connection=conn;
	connect(this,SIGNAL(newData(QByteArray)),connection,SLOT(onNewData(QByteArray)),Qt::QueuedConnection);
	connect(this,&ServerConnectionSocketWrap::connectionError,
		connection,&ServerConnection::onConnectionError,Qt::QueuedConnection);
}

ServerConnectionSocketWrap::~ServerConnectionSocketWrap()
{
	if(!netSock)return;
	if(connType==ServerConnection::TcpSock)
		netSock->deleteLater();
	else if(connType==ServerConnection::UnixSock)
		localSock->deleteLater();
	else delete stdio;
}

void ServerConnectionSocketWrap::startConnectLocal()
{
	connType=ServerConnection::UnixSock;
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
	connType=ServerConnection::TcpSock;
	netSock=new QSslSocket(this);
	netSock->setProxy(connection->proxy);
	netSock->setPeerVerifyMode(QSslSocket::VerifyNone);
	netSock->setSocketOption(QAbstractSocket::LowDelayOption,1);
	connect(netSock,&QSslSocket::encrypted,connection,&ServerConnection::onNetSocketConnected,Qt::QueuedConnection);
	connect(netSock,&QSslSocket::disconnected,connection,&ServerConnection::onDevDisconnected,Qt::QueuedConnection);
	connect(netSock,static_cast<void(QSslSocket::*)(QAbstractSocket::SocketError)>(&QSslSocket::error),
		this,&ServerConnectionSocketWrap::onNetError,Qt::DirectConnection);
	connect(netSock,static_cast<void(QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&ServerConnectionSocketWrap::onSslError,Qt::DirectConnection);
	connect(netSock,SIGNAL(readyRead()),this,SLOT(onNetReadyRead()),Qt::DirectConnection);
	netSock->connectToHostEncrypted(connection->mHost,connection->mPort);
}

void ServerConnectionSocketWrap::startConnectStdio()
{
	connType=ServerConnection::Stdio;
	stdio=new StdInOut;
	connect(&stdio->stdinFile,&QFile::aboutToClose,
		connection,&ServerConnection::onDevDisconnected,Qt::QueuedConnection);
	if(stdio->stdinNotif)
		stdio->notifConn=connect(stdio->stdinNotif,SIGNAL(activated(int)),this,
			SLOT(onStdioReadyRead()),Qt::DirectConnection);
	if(!stdio->stdinFile.isOpen())
		emit connectionError();
	else QMetaObject::invokeMethod(connection,"onStdioConnected",Qt::QueuedConnection);
}

void ServerConnectionSocketWrap::disconnectFromServer()
{
	if(!connection->noDebug)
		qDebug()<<"ServerConnectionSocketWrap::disconnectFromServer";
	if(connType==ServerConnection::TcpSock)
		netSock->disconnectFromHost();
	else if(connType==ServerConnection::UnixSock)
		localSock->disconnectFromServer();
	else
	{
		stdio->stdoutFile.close();
		disconnect(stdio->notifConn);
		stdio->stdinFile.close();
	}
}

void ServerConnectionSocketWrap::writeData(QByteArray data)
{
	if(connType==ServerConnection::TcpSock)
	{
		netSock->write(data);
		netSock->flush();
	}
	else if(connType==ServerConnection::UnixSock)
	{
		localSock->write(data);
		localSock->flush();
	}
	else
	{
		if(stdio->stdoutFile.write(data)!=data.size())
			emit connectionError();
		stdio->stdoutFile.flush();
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

void ServerConnectionSocketWrap::onStdioReadyRead()
{
	QByteArray data=stdio->stdinFile.readAll();
	if(!data.isEmpty())
		emit newData(data);
}
