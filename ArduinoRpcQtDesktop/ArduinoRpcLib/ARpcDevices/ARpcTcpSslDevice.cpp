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

#include "ARpcTcpSslDevice.h"
#include <QHostAddress>

ARpcTcpSslDevice::ARpcTcpSslDevice(const QHostAddress &addr,QObject *parent)
	:ARpcRealDevice(parent)
{
	mAddress=addr;
	socket=new QSslSocket(this);
	socket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
	socket->setPeerVerifyMode(QSslSocket::VerifyNone);
	reconnectTimer.setInterval(60*1000);
	reconnectTimer.setSingleShot(false);

	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTcpSslDevice::onReconnectTimer);
	connect(socket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected);
	connect(socket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected);
	connect(socket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted);
	connect(socket,&QSslSocket::destroyed,this,&ARpcTcpSslDevice::onSocketDestroyed);
	connect(socket,static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&ARpcTcpSslDevice::onSslErrors);
	connect(socket,&QSslSocket::readyRead,this,&ARpcTcpSslDevice::onReadyRead);

	reconnectTimer.start();
	onReconnectTimer();
}

ARpcTcpSslDevice::ARpcTcpSslDevice(QSslSocket *s,QObject *parent)
	:ARpcRealDevice(parent)
{
	socket=s;
	reconnectTimer.setInterval(10*1000);
	reconnectTimer.setSingleShot(false);
	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTcpSslDevice::onReconnectTimer);
	if(socket)
	{
		mAddress=socket->peerAddress();
		socket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
		socket->setPeerVerifyMode(QSslSocket::VerifyNone);
		connect(socket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected,Qt::DirectConnection);
		connect(socket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected,Qt::DirectConnection);
		connect(socket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted,Qt::DirectConnection);
		connect(socket,&QSslSocket::destroyed,this,&ARpcTcpSslDevice::onSocketDestroyed,Qt::DirectConnection);
		connect(socket,static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
			this,&ARpcTcpSslDevice::onSslErrors,Qt::DirectConnection);
		connect(socket,&QSslSocket::readyRead,this,&ARpcTcpSslDevice::onReadyRead,Qt::DirectConnection);
		if(!socket->isEncrypted())
		{
			reconnectTimer.start();
			onReconnectTimer();
		}
	}
}

void ARpcTcpSslDevice::setNewSocket(QSslSocket *s,const QUuid &newId,const QString &newName)
{
	if(socket)
		socket->disconnectFromHost();
	mAddress.clear();
	socket=s;
	if(socket)
	{
		socket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
		socket->setPeerVerifyMode(QSslSocket::VerifyNone);
		mAddress=socket->peerAddress();
		connect(socket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected,Qt::DirectConnection);
		connect(socket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected,Qt::DirectConnection);
		connect(socket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted,Qt::DirectConnection);
		connect(socket,&QSslSocket::destroyed,this,&ARpcTcpSslDevice::onSocketDestroyed,Qt::DirectConnection);
		connect(socket,static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
			this,&ARpcTcpSslDevice::onSslErrors,Qt::DirectConnection);
		connect(socket,&QSslSocket::readyRead,this,&ARpcTcpSslDevice::onReadyRead,Qt::DirectConnection);
	}
	streamParser.reset();
	if(isConnected())
		resetIdentification(newId,newName);
}

bool ARpcTcpSslDevice::writeMsg(const ARpcMessage &m)
{
	if(!isConnected())
		return false;
	QByteArray data=(msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8();
	if(socket->write(data)!=data.size())
		return false;
	return socket->flush();
}

bool ARpcTcpSslDevice::isConnected()
{
	return socket&&socket->state()==QAbstractSocket::ConnectedState&&socket->isEncrypted();
}

QHostAddress ARpcTcpSslDevice::address() const
{
	return mAddress;
}

void ARpcTcpSslDevice::onReconnectTimer()
{
	if(!socket)return;
	socket->disconnectFromHost();
	socket->waitForDisconnected(1000);
	if(!mAddress.isNull())
		socket->connectToHostEncrypted(mAddress.toString(),ARpcConfig::netDeviceSslPort);
}

void ARpcTcpSslDevice::onSocketConnected()
{
	reconnectTimer.stop();
}

void ARpcTcpSslDevice::onSocketEncrypted()
{
	emit connected();
}

void ARpcTcpSslDevice::onSocketDestroyed()
{
	socket=0;
	emit disconnected();
	streamParser.reset();
}

void ARpcTcpSslDevice::onSslErrors()
{
	socket->disconnectFromHost();
}

void ARpcTcpSslDevice::onSocketDisonnected()
{
	resetIdentification();
	emit disconnected();
	reconnectTimer.start();
}

void ARpcTcpSslDevice::onReadyRead()
{
	QByteArray data=socket->readAll();
	if(!data.isEmpty())
		streamParser.pushData(QString::fromUtf8(data));
}
