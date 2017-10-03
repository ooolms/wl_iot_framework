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
	retryTimer.setInterval(60*1000);
	retryTimer.setSingleShot(false);

	connect(&retryTimer,&QTimer::timeout,this,&ARpcTcpDevice::onRetryTimer);
	connect(socket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected);
	connect(socket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected);
	connect(socket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead);

	retryTimer.start();
	onRetryTimer();
}

ARpcTcpDevice::ARpcTcpDevice(QTcpSocket *s,QObject *parent)
	:ARpcRealDevice(parent)
{
	socket=s;
	if(socket)
	{
		mAddress=socket->peerAddress();
		socket->setParent(this);
	}
	retryTimer.setInterval(60*1000);
	retryTimer.setSingleShot(false);

	connect(&retryTimer,&QTimer::timeout,this,&ARpcTcpDevice::onRetryTimer);
	if(socket)
	{
		connect(socket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected);
		connect(socket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected);
		connect(socket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead);
	}

	retryTimer.start();
	onRetryTimer();
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
		mAddress=socket->peerAddress();
		connect(socket,&QTcpSocket::connected,this,&ARpcTcpDevice::onSocketConnected);
		connect(socket,&QTcpSocket::disconnected,this,&ARpcTcpDevice::onSocketDisonnected);
		connect(socket,&QTcpSocket::readyRead,this,&ARpcTcpDevice::onReadyRead);
	}
	resetIdentification(newId,newName);
}

bool ARpcTcpDevice::writeMsg(const ARpcMessage &m)
{
	if(!socket)return false;
	if(socket->state()!=QAbstractSocket::ConnectedState)return false;
	QByteArray data=(msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8();
	if(socket->write(data)!=data.size())return false;
	return socket->flush();
}

bool ARpcTcpDevice::isConnected()
{
	if(!socket)return false;
	return socket->state()==QAbstractSocket::ConnectedState;
}

QHostAddress ARpcTcpDevice::address()const
{
	return mAddress;
}

void ARpcTcpDevice::onRetryTimer()
{
	if(!socket)return;
	if(socket->state()!=QAbstractSocket::UnconnectedState)return;
	socket->connectToHost(mAddress,ARpcConfig::netDevicePort);
}

void ARpcTcpDevice::onSocketConnected()
{
	emit connected();
	retryTimer.stop();
}

void ARpcTcpDevice::onSocketDisonnected()
{
	emit disconnected();
	resetIdentification();
	retryTimer.start();
}

void ARpcTcpDevice::onReadyRead()
{
	QByteArray data=socket->readAll();
	if(!data.isEmpty())streamParser.pushData(QString::fromUtf8(data));
}
