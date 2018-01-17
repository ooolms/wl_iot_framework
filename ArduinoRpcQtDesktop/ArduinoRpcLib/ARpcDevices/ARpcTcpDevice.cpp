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

#include "ARpcTcpDevice.h"
#include <arpa/inet.h>

ARpcTcpDevice::ARpcTcpDevice(const QString &addr,QObject *parent)
	:ARpcRealDevice(parent)
{
	mAddress=addr;
	mSocket=new QTcpSocket(this);
	reconnectTimer.setInterval(60*1000);
	reconnectTimer.setSingleShot(false);

	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTcpDevice::onReconnectTimer);
	connect(mSocket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected);
	connect(mSocket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected);
	connect(mSocket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead);

	reconnectTimer.start();
	onReconnectTimer();
}

ARpcTcpDevice::ARpcTcpDevice(qintptr s,QObject *parent)
	:ARpcRealDevice(parent)
{
	mSocket=new QTcpSocket(this);
	mSocket->setSocketDescriptor(s);
	readAddrFromSocket(s);
	reconnectTimer.setInterval(10*1000);
	reconnectTimer.setSingleShot(false);
	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTcpDevice::onReconnectTimer);
	connect(mSocket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::destroyed,this,&ARpcTcpDevice::onSocketDestroyed,Qt::DirectConnection);
	if(mSocket->state()!=QAbstractSocket::ConnectedState)
	{
		reconnectTimer.start();
		onReconnectTimer();
	}
}

void ARpcTcpDevice::setNewSocket(qintptr s,const QUuid &newId,const QString &newName)
{
	mSocket->disconnectFromHost();
	delete mSocket;
	mSocket=new QTcpSocket(this);
	mSocket->setSocketDescriptor(s);
	readAddrFromSocket(s);
	connect(mSocket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::destroyed,this,&ARpcTcpDevice::onSocketDestroyed,Qt::DirectConnection);
	resetIdentification(newId,newName);
	streamParser.reset();
}

void ARpcTcpDevice::setNewSocket(QTcpSocket *s,const QUuid &newId,const QString &newName)
{
	mSocket->disconnectFromHost();
	delete mSocket;
	mSocket=s;
	mSocket->setParent(this);
	readAddrFromSocket(s->socketDescriptor());
	reconnectTimer.stop();
	connect(mSocket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::destroyed,this,&ARpcTcpDevice::onSocketDestroyed,Qt::DirectConnection);
	resetIdentification(newId,newName);
	streamParser.reset();
}

bool ARpcTcpDevice::writeMsg(const ARpcMessage &m)
{
	if(!isConnected())
		return false;
	QByteArray data=ARpcStreamParser::dump(m).toUtf8();
	if(mSocket->write(data)!=data.size())
	{
		qDebug()<<mSocket->error();
		return false;
	}
	if(!mSocket->flush())
	{
		qDebug()<<mSocket->error();
		return false;
	}
	return true;
}

bool ARpcTcpDevice::isConnected()
{
	return mSocket&&mSocket->state()==QAbstractSocket::ConnectedState;
}

QString ARpcTcpDevice::address() const
{
	return mAddress;
}

qintptr ARpcTcpDevice::socket()
{
	return mSocket->socketDescriptor();
}

QTcpSocket *ARpcTcpDevice::takeSocket()
{
	QTcpSocket *s=mSocket;
	mSocket->setParent(0);
	mSocket=0;
	return s;
}

void ARpcTcpDevice::waitForConnected()
{
	mSocket->waitForConnected();
}

void ARpcTcpDevice::disconnectFromHost()
{
	if(!mSocket)return;
	if(mSocket->state()==QAbstractSocket::ConnectedState)
		mSocket->disconnectFromHost();
}

void ARpcTcpDevice::reconnect()
{
	if(!mSocket)return;
	onReconnectTimer();
}

void ARpcTcpDevice::onReconnectTimer()
{
	if(mSocket->state()!=QAbstractSocket::UnconnectedState)
	{
		mSocket->disconnectFromHost();
		mSocket->waitForDisconnected(1000);
	}
	if(!mAddress.isNull())
		mSocket->connectToHost(mAddress,ARpcConfig::netDevicePort);
}

void ARpcTcpDevice::onSocketConnected()
{
	reconnectTimer.stop();
	emit connected();
}

void ARpcTcpDevice::onSocketDisonnected()
{
	resetIdentification();
	emit disconnected();
	reconnectTimer.start();
}

void ARpcTcpDevice::onReadyRead()
{
	QByteArray data=mSocket->readAll();
	if(!data.isEmpty())
		streamParser.pushData(QString::fromUtf8(data));
}

void ARpcTcpDevice::onSocketDestroyed()
{
	mSocket=0;
	emit disconnected();
	streamParser.reset();
}

void ARpcTcpDevice::readAddrFromSocket(qintptr s)
{
	socklen_t len;
	struct sockaddr_storage addr;
	len=sizeof addr;
	getpeername(s,(struct sockaddr*)&addr,&len);

	char ipstr[INET6_ADDRSTRLEN];
	qDebug()<<addr.ss_family;
	if(addr.ss_family==AF_INET)
	{
		struct sockaddr_in *s=(struct sockaddr_in*)&addr;
		mPort=ntohs(s->sin_port);
		inet_ntop(AF_INET,&s->sin_addr,ipstr,sizeof ipstr);
		mAddress=QString::fromUtf8(ipstr);
	}
	else if(addr.ss_family==AF_INET6)// AF_INET6
	{
		struct sockaddr_in6 *s=(struct sockaddr_in6*)&addr;
		mPort=ntohs(s->sin6_port);
		inet_ntop(AF_INET6,&s->sin6_addr,ipstr,sizeof ipstr);
		mAddress=QString::fromUtf8(ipstr);
	}
	else mAddress=QHostAddress((sockaddr*)&addr).toString();
	qDebug()<<"Sock state: "<<mSocket->state();
	qDebug()<<"Peer address: "<<mAddress;
	qDebug()<<"Peer port: "<<mPort;
}
