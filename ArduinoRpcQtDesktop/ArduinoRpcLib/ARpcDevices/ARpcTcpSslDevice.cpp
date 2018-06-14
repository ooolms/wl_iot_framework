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

//TODO в процессе onSocketEncrypted не передаются данные от клиента!!!
//соответственно, нельзя обрабатывать сигнал connected() и обмениваться сразу данными
ARpcTcpSslDevice::ARpcTcpSslDevice(const QHostAddress &addr,QObject *parent)
	:ARpcRealDevice(parent)
{
	mAddress=addr;
	mSocket=new QSslSocket(this);
	mSocket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
	mSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
	reconnectTimer.setInterval(60*1000);
	reconnectTimer.setSingleShot(false);

	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTcpSslDevice::onReconnectTimer);
	connect(mSocket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected);
	connect(mSocket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected);
	connect(mSocket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted);
	connect(mSocket,&QSslSocket::destroyed,this,&ARpcTcpSslDevice::onSocketDestroyed);
	connect(mSocket,static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&ARpcTcpSslDevice::onSslErrors);
	connect(mSocket,&QSslSocket::readyRead,this,&ARpcTcpSslDevice::onReadyRead);

	reconnectTimer.start();
}

ARpcTcpSslDevice::ARpcTcpSslDevice(QSslSocket *s,QObject *parent)
	:ARpcRealDevice(parent)
{
	mSocket=s;
	reconnectTimer.setInterval(10*1000);
	reconnectTimer.setSingleShot(false);
	connect(&reconnectTimer,&QTimer::timeout,this,&ARpcTcpSslDevice::onReconnectTimer);
	if(mSocket)
	{
		mAddress=mSocket->peerAddress();
		mSocket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
		mSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
		connect(mSocket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected,Qt::DirectConnection);
		connect(mSocket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected,Qt::DirectConnection);
		connect(mSocket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted,Qt::DirectConnection);
		connect(mSocket,&QSslSocket::destroyed,this,&ARpcTcpSslDevice::onSocketDestroyed,Qt::DirectConnection);
		connect(mSocket,static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
			this,&ARpcTcpSslDevice::onSslErrors,Qt::DirectConnection);
		connect(mSocket,&QSslSocket::readyRead,this,&ARpcTcpSslDevice::onReadyRead,Qt::DirectConnection);
		if(!mSocket->isEncrypted())
		{
			reconnectTimer.start();
			onReconnectTimer();
		}
	}
}

void ARpcTcpSslDevice::setNewSocket(QSslSocket *s,const QUuid &newId,const QByteArray &newName)
{
	if(mSocket)
		mSocket->disconnectFromHost();
	mAddress.clear();
	mSocket=s;
	if(mSocket)
	{
		mSocket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
		mSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
		mAddress=mSocket->peerAddress();
		connect(mSocket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected,Qt::DirectConnection);
		connect(mSocket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected,Qt::DirectConnection);
		connect(mSocket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted,Qt::DirectConnection);
		connect(mSocket,&QSslSocket::destroyed,this,&ARpcTcpSslDevice::onSocketDestroyed,Qt::DirectConnection);
		connect(mSocket,static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
			this,&ARpcTcpSslDevice::onSslErrors,Qt::DirectConnection);
		connect(mSocket,&QSslSocket::readyRead,this,&ARpcTcpSslDevice::onReadyRead,Qt::DirectConnection);
	}
	streamParser.reset();
	if(isConnected())
		resetIdentification(newId,newName);
}

bool ARpcTcpSslDevice::writeMsg(const ARpcMessage &m)
{
	if(!isConnected())
		return false;
	QByteArray data=ARpcStreamParser::dump(m);
	if(mSocket->write(data)!=data.size())
		return false;
	return mSocket->flush();
}

bool ARpcTcpSslDevice::isConnected()
{
	return mSocket&&mSocket->state()==QAbstractSocket::ConnectedState&&mSocket->isEncrypted();
}

QHostAddress ARpcTcpSslDevice::address() const
{
	return mAddress;
}

void ARpcTcpSslDevice::syncFailed()
{
	if(!mSocket)return;
	mSocket->disconnectFromHost();
	reconnectTimer.start();
}

void ARpcTcpSslDevice::onReconnectTimer()
{
	if(!mSocket)return;
	if(mSocket->state()!=QAbstractSocket::UnconnectedState)
	{
		mSocket->disconnectFromHost();
		mSocket->waitForDisconnected(1000);
	}
	if(!mAddress.isNull())
		mSocket->connectToHostEncrypted(mAddress.toString(),ARpcConfig::netDeviceSslPort);
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
	mSocket=0;
	emit disconnected();
	streamParser.reset();
}

void ARpcTcpSslDevice::onSslErrors()
{
	mSocket->disconnectFromHost();
}

void ARpcTcpSslDevice::onSocketDisonnected()
{
	emit disconnected();
	streamParser.reset();
	reconnectTimer.start();
}

void ARpcTcpSslDevice::onReadyRead()
{
	QByteArray data=mSocket->readAll();
	if(!data.isEmpty())
		streamParser.pushData(data);
}
