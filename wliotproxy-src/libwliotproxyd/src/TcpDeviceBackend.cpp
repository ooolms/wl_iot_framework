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

#include "wliot/devices/TcpDeviceBackend.h"
#include "wliot/WLIOTProtocolDefs.h"

#if defined Q_OS_WIN
#include <Winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

using namespace WLIOT;

const QByteArray TcpDeviceBackend::devType=QByteArray("tcp");

TcpDeviceBackend::TcpDeviceBackend(const QString &addr,QObject *parent)
	:ILowLevelDeviceBackend(parent)
{
	mAddress=addr;
	mSocket=new QTcpSocket(this);
	setupSocket();

	if(!mAddress.isNull())
		startSocketConnection();
}

TcpDeviceBackend::TcpDeviceBackend(qintptr s,QObject *parent)
	:ILowLevelDeviceBackend(parent)
{
	mSocket=new QTcpSocket(this);
	mSocket->setSocketDescriptor(s);
	readAddrFromSocket(s);
	setupSocket();
}

TcpDeviceBackend::TcpDeviceBackend(QObject *parent)
	:ILowLevelDeviceBackend(parent)
{
	mSocket=nullptr;
}

void TcpDeviceBackend::setupSocket()
{
	if(!mSocket)return;
	connect(mSocket,&QTcpSocket::connected,this,&TcpDeviceBackend::onSocketConnected,Qt::DirectConnection);
	connect(mSocket,&QTcpSocket::disconnected,this,&TcpDeviceBackend::onSocketDisonnected,Qt::DirectConnection);
	connect(mSocket,SIGNAL(readyRead()),this,SLOT(onReadyRead()),Qt::DirectConnection);
}

QString TcpDeviceBackend::address()const
{
	return mAddress;
}

qintptr TcpDeviceBackend::socketDescriptor()
{
	return mSocket->socketDescriptor();
}

bool TcpDeviceBackend::waitForConnected(int msecs)
{
	return mSocket->waitForConnected(msecs);
}

void TcpDeviceBackend::disconnectFromHost()
{
	if(!mSocket)return;
	if(mSocket->state()==QAbstractSocket::ConnectedState)
		mSocket->disconnectFromHost();
}

bool TcpDeviceBackend::writeData(const QByteArray &data)
{
	return mSocket->write(data)==data.size();
}

bool TcpDeviceBackend::flush()
{
	return mSocket->flush();
}

bool TcpDeviceBackend::isConnected()const
{
	return mSocket->state()==QTcpSocket::ConnectedState;
}

void TcpDeviceBackend::forceDisconnect()
{
	mSocket->disconnectFromHost();
}

QByteArray TcpDeviceBackend::type()const
{
	return devType;
}

QByteArray TcpDeviceBackend::portOrAddress()const
{
	return address().toUtf8();
}

void TcpDeviceBackend::startSocketConnection()
{
	mSocket->connectToHost(mAddress,WLIOTProtocolDefs::netDevicePort);
}

void TcpDeviceBackend::processOnSocketConnected()
{
	emit connected();
}

void TcpDeviceBackend::onSocketConnected()
{
	processOnSocketConnected();
}

void TcpDeviceBackend::onSocketDisonnected()
{
	emit disconnected();
}

void TcpDeviceBackend::onReadyRead()
{
	QByteArray data=mSocket->readAll();
	if(!data.isEmpty())
		emit newData(data);
}

void TcpDeviceBackend::readAddrFromSocket(qintptr s)
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
