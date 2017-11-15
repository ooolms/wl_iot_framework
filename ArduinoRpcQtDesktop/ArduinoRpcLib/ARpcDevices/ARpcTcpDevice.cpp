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

ARpcTcpDevice::ARpcTcpDevice(const QHostAddress &addr,QObject *parent)
	:ARpcRealDevice(parent)
{
	mAddress=addr;
	socket=new QTcpSocket(this);
	reconnectTimer.setInterval(60*1000);
	reconnectTimer.setSingleShot(false);

	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTcpDevice::onReconnectTimer);
	connect(socket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected);
	connect(socket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected);
	connect(socket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead);

	reconnectTimer.start();
	onReconnectTimer();
}

ARpcTcpDevice::ARpcTcpDevice(QTcpSocket *s,QObject *parent)
	:ARpcRealDevice(parent)
{
	socket=s;
	reconnectTimer.setInterval(10*1000);
	reconnectTimer.setSingleShot(false);
	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTcpDevice::onReconnectTimer);
	if(socket)
	{
		mAddress=socket->peerAddress();
		socket->setParent(this);
		connect(socket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected,Qt::DirectConnection);
		connect(socket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected,Qt::DirectConnection);
		connect(socket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead,Qt::DirectConnection);
		if(socket->state()!=QAbstractSocket::ConnectedState)
		{
			reconnectTimer.start();
			onReconnectTimer();
		}
	}
}

void ARpcTcpDevice::setNewSocket(QTcpSocket *s,const QUuid &newId,const QString &newName)
{
	if(socket)
	{
		socket->disconnectFromHost();
		delete socket;
	}
	mAddress.clear();
	socket=s;
	if(socket)
	{
		socket->setParent(this);
		mAddress=socket->peerAddress();
		connect(socket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected);
		connect(socket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected);
		connect(socket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead);
	}
	streamParser.reset();
	if(socket->state()==QAbstractSocket::ConnectedState)
		resetIdentification(newId,newName);
}

bool ARpcTcpDevice::writeMsg(const ARpcMessage &m)
{
	if(!isConnected())
		return false;
	QByteArray data=(msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8();
	if(socket->write(data)!=data.size())
		return false;
	return socket->flush();
}

bool ARpcTcpDevice::isConnected()
{
	return socket&&socket->state()==QAbstractSocket::ConnectedState;
}

QHostAddress ARpcTcpDevice::address() const
{
	return mAddress;
}

void ARpcTcpDevice::onReconnectTimer()
{
	if(!socket)
		return;
	socket->disconnectFromHost();
	socket->waitForDisconnected(1000);
	if(!mAddress.isNull())
		socket->connectToHost(mAddress,ARpcConfig::netDevicePort);
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
	QByteArray data=socket->readAll();
	if(!data.isEmpty())
		streamParser.pushData(QString::fromUtf8(data));
}
