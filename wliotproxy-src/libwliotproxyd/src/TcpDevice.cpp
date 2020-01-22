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
#include "wliot/WLIOTProtocolDefs.h"

#if defined Q_OS_WIN
#include <Winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

TcpDevice::TcpDevice(const QString &addr,QObject *parent)
	:ILowLevelDeviceBackend(parent)
{
	mAddress=addr;
	mSocket=new QTcpSocket(this);
	setupSocket();

	if(!mAddress.isNull())
		startSocketConnection();
}

TcpDevice::TcpDevice(qintptr s,QObject *parent)
	:ILowLevelDeviceBackend(parent)
{
	mSocket=new QTcpSocket(this);
	mSocket->setSocketDescriptor(s);
	readAddrFromSocket(s);
	setupSocket();
}

TcpDevice::TcpDevice(QObject *parent)
	:ILowLevelDeviceBackend(parent)
{
	mSocket=nullptr;
}

void TcpDevice::setupSocket()
{
	if(!mSocket)return;
	connect(mSocket,&QTcpSocket::connected,this,&TcpDevice::onSocketConnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::disconnected,this,&TcpDevice::onSocketDisonnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::readyRead,this,&TcpDevice::onReadyRead,Qt::DirectConnection);
}

QString TcpDevice::address()const
{
	return mAddress;
}

qintptr TcpDevice::socketDescriptor()
{
	return mSocket->socketDescriptor();
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

bool TcpDevice::writeData(const QByteArray &data)
{
	return mSocket->write(data)==data.size();
}

bool TcpDevice::flush()
{
	return mSocket->flush();
}

bool TcpDevice::isConnected()const
{
	return mSocket->state()==QTcpSocket::ConnectedState;
}

void TcpDevice::forceDisconnect()
{
	mSocket->disconnectFromHost();
}

void TcpDevice::startSocketConnection()
{
	mSocket->connectToHost(mAddress,WLIOTProtocolDefs::netDevicePort);
}

void TcpDevice::processOnSocketConnected()
{
	emit connected();
}

void TcpDevice::onSocketConnected()
{
	processOnSocketConnected();
}

void TcpDevice::onSocketDisonnected()
{
	emit disconnected();
}

void TcpDevice::onReadyRead()
{
	QByteArray data=mSocket->readAll();
	if(!data.isEmpty())
		emit newData(data);
}

void TcpDevice::readAddrFromSocket(qintptr s)
{
	socklen_t len;
	struct sockaddr_storage addr;
	len=sizeof addr;
	getpeername((int)s,(struct sockaddr*)&addr,&len);

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
