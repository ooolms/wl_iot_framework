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

#include "wliot/devices/TcpDevice.h"

#if defined Q_WS_WIN || defined WINVER
#include <Winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

TcpDevice::TcpDevice(const QString &addr,QObject *parent)
	:RealDevice(parent)
{
	mAddress=addr;
	mSocket=new QTcpSocket(this);
	setupSocket();
	connect(&streamParser,&StreamParser::newMessage,this,&TcpDevice::onNewMessage);

	if(!mAddress.isNull())
		startSocketConnection();
}

TcpDevice::TcpDevice(qintptr s,QObject *parent)
	:RealDevice(parent)
{
	mSocket=new QTcpSocket(this);
	mSocket->setSocketDescriptor(s);
	readAddrFromSocket(s);
	setupSocket();
	connect(&streamParser,&StreamParser::newMessage,this,&TcpDevice::onNewMessage);

	if(mSocket->state()==QAbstractSocket::ConnectedState)
		onConnected();
}

TcpDevice::TcpDevice(QObject *parent)
	:RealDevice(parent)
{
	mSocket=0;
	connect(&streamParser,&StreamParser::newMessage,this,&TcpDevice::onNewMessage);
}

void TcpDevice::setupSocket()
{
	if(!mSocket)return;
	connect(mSocket,&QTcpSocket::connected,this,&TcpDevice::onSocketConnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::disconnected,this,&TcpDevice::onSocketDisonnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::readyRead,this,&TcpDevice::onReadyRead,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::destroyed,this,&TcpDevice::onSocketDestroyed,Qt::DirectConnection);
}

void TcpDevice::setNewSocket(qintptr s,const QUuid &newId,const QByteArray &newName)
{
	if(mSocket)
	{
		mSocket->disconnect(this);
		mSocket->disconnectFromHost();
		mSocket->deleteLater();
	}
	mSocket=new QTcpSocket(this);
	mSocket->setSocketDescriptor(s);
	readAddrFromSocket(s);
	setupSocket();
	if(mSocket->state()==QAbstractSocket::ConnectedState&&!isConnected())
		onConnected();
	resetIdentification(newId,newName);
	streamParser.reset();
	onDeviceReset();
}

void TcpDevice::setNewSocket(QTcpSocket *s,const QUuid &newId,const QByteArray &newName)
{
	if(mSocket)
	{
		mSocket->disconnect(this);
		mSocket->disconnectFromHost();
		mSocket->deleteLater();
	}
	mAddress.clear();
	mSocket=s;
	if(!mSocket)return;
	mSocket->setParent(this);
	readAddrFromSocket(s->socketDescriptor());
	setupSocket();
	if(mSocket->state()==QAbstractSocket::ConnectedState&&!isConnected())
		onConnected();
	resetIdentification(newId,newName);
	streamParser.reset();
	onDeviceReset();
}

bool TcpDevice::writeMsgToDevice(const Message &m)
{
	if(!isConnected())
		return false;
	QByteArray data=m.dump();
	if(mSocket->write(data)!=data.size())
	{
		qDebug()<<mSocket->errorString();
		return false;
	}
	mSocket->flush();
	return true;
}

QString TcpDevice::address()const
{
	return mAddress;
}

qintptr TcpDevice::socketDescriptor()
{
	return mSocket->socketDescriptor();
}

QTcpSocket* TcpDevice::takeSocket()
{
	QTcpSocket *s=mSocket;
	mSocket->setParent(0);
	mSocket->disconnect(this);
	mSocket=0;
	return s;
}

bool TcpDevice::waitForConnected()
{
	return mSocket->waitForConnected();
}

void TcpDevice::disconnectFromHost()
{
	if(!mSocket)return;
	if(mSocket->state()==QAbstractSocket::ConnectedState)
		mSocket->disconnectFromHost();
}

void TcpDevice::syncFailed()
{
	if(!mSocket)return;
	disconnectFromHost();
}

void TcpDevice::startSocketConnection()
{
	mSocket->connectToHost(mAddress,WLIOTProtocolDefs::netDevicePort);
}

void TcpDevice::processOnSocketConnected()
{
	onConnected();
	identify();
}

void TcpDevice::onSocketConnected()
{
	processOnSocketConnected();
}

void TcpDevice::onSocketDisonnected()
{
	streamParser.reset();
	onDisconnected();
}

void TcpDevice::onReadyRead()
{
	QByteArray data=mSocket->readAll();
	if(!data.isEmpty())
		streamParser.pushData(data);
}

void TcpDevice::onSocketDestroyed()
{
	mSocket=0;
	streamParser.reset();
	onDisconnected();
}

void TcpDevice::readAddrFromSocket(qintptr s)
{
	socklen_t len;
	struct sockaddr_storage addr;
	len=sizeof addr;
	getpeername(s,(struct sockaddr*)&addr,&len);

	char ipstr[INET6_ADDRSTRLEN];
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
}
