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
	retryTimer.setInterval(60*1000);
	retryTimer.setSingleShot(false);

	connect(&retryTimer,&QTimer::timeout,this,&ARpcTcpSslDevice::onRetryTimer);
	connect(socket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected);
	connect(socket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected);
	connect(socket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted);
	connect(socket,static_cast<void (QSslSocket::*)(
		const QList<QSslError>&)>(&QSslSocket::sslErrors),this,
		&ARpcTcpSslDevice::onSslErrors);
	connect(socket,&QSslSocket::readyRead,this,&ARpcTcpSslDevice::onReadyRead);

	retryTimer.start();
	onRetryTimer();
}

ARpcTcpSslDevice::ARpcTcpSslDevice(QSslSocket *s,QObject *parent)
	:ARpcRealDevice(parent)
{
	socket=s;
	if(socket)
	{
		mAddress=socket->peerAddress();
		socket->setParent(this);
		socket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
		socket->setPeerVerifyMode(QSslSocket::VerifyNone);
	}
	connect(&retryTimer,&QTimer::timeout,this,&ARpcTcpSslDevice::onRetryTimer);
	if(socket)
	{
		connect(socket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected);
		connect(socket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected);
		connect(socket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted);
		connect(socket,static_cast<void (QSslSocket::*)(
			const QList<QSslError>&)>(&QSslSocket::sslErrors),this,
			&ARpcTcpSslDevice::onSslErrors);
		connect(socket,&QSslSocket::readyRead,this,&ARpcTcpSslDevice::onReadyRead);
	}
	retryTimer.start();
	onRetryTimer();
}

void ARpcTcpSslDevice::setNewSocket(QSslSocket *s,const QUuid &newId,const QString &newName)
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
		socket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
		socket->setPeerVerifyMode(QSslSocket::VerifyNone);
		mAddress=socket->peerAddress();
		connect(socket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected);
		connect(socket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected);
		connect(socket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted);
		connect(socket,static_cast<void (QSslSocket::*)(
			const QList<QSslError>&)>(&QSslSocket::sslErrors),this,
			&ARpcTcpSslDevice::onSslErrors);
		connect(socket,&QSslSocket::readyRead,this,&ARpcTcpSslDevice::onReadyRead);
	}
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
	return socket->state()==QAbstractSocket::ConnectedState&&socket->isEncrypted();
}

QHostAddress ARpcTcpSslDevice::address() const
{
	return mAddress;
}

void ARpcTcpSslDevice::onRetryTimer()
{
	socket->disconnectFromHost();
	socket->waitForDisconnected(1000);
	socket->connectToHostEncrypted(mAddress.toString(),ARpcConfig::netDeviceSslPort);
}

void ARpcTcpSslDevice::onSocketConnected()
{
	retryTimer.stop();
}

void ARpcTcpSslDevice::onSocketEncrypted()
{
	emit connected();
}

void ARpcTcpSslDevice::onSslErrors()
{
	socket->disconnectFromHost();
	retryTimer.start();
}

void ARpcTcpSslDevice::onSocketDisonnected()
{
	emit disconnected();
	resetIdentification();
	retryTimer.start();
}

void ARpcTcpSslDevice::onReadyRead()
{
	QByteArray data=socket->readAll();
	if(!data.isEmpty())streamParser.pushData(QString::fromUtf8(data));
}
