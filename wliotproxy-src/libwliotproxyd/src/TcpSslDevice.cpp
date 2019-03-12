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

#include "wliot/devices/TcpSslDevice.h"
#include <QHostAddress>

TcpSslDevice::TcpSslDevice(const QString &addr,QObject *parent)
	:TcpDevice(parent)
{
	mAddress=addr;
	mSocket=new QSslSocket(this);
	((QSslSocket*)mSocket)->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
	((QSslSocket*)mSocket)->setPeerVerifyMode(QSslSocket::VerifyNone);
	setupSocket();
	connect(sslSocket(),&QSslSocket::encrypted,this,&TcpSslDevice::onSocketEncrypted);
	connect(sslSocket(),static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&TcpSslDevice::onSslErrors);

	if(!mAddress.isNull())
		startSocketConnection();
}

TcpSslDevice::TcpSslDevice(qintptr s,QObject *parent)
	:TcpDevice(parent)
{
	mSocket=new QSslSocket(this);
	mSocket->setSocketDescriptor(s);
	((QSslSocket*)mSocket)->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
	((QSslSocket*)mSocket)->setPeerVerifyMode(QSslSocket::VerifyNone);
	readAddrFromSocket(s);
	setupSocket();
	connect(sslSocket(),&QSslSocket::encrypted,this,&TcpSslDevice::onSocketEncrypted,Qt::QueuedConnection);
	connect(sslSocket(),static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&TcpSslDevice::onSslErrors,Qt::DirectConnection);

	if(mSocket->state()==QAbstractSocket::ConnectedState)
		((QSslSocket*)mSocket)->startServerEncryption();
}

void TcpSslDevice::setNewSocket(qintptr s,const QUuid &newId,const QByteArray &newName)
{
	mSocket->disconnect(this);
	mSocket->disconnectFromHost();
	delete mSocket;
	mSocket=new QSslSocket(this);
	mSocket->setSocketDescriptor(s);
	sslSocket()->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
	sslSocket()->setPeerVerifyMode(QSslSocket::VerifyNone);
	readAddrFromSocket(s);
	setupSocket();
	connect(sslSocket(),&QSslSocket::encrypted,this,&TcpSslDevice::onSocketEncrypted,Qt::QueuedConnection);
	connect(sslSocket(),static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&TcpSslDevice::onSslErrors,Qt::DirectConnection);
	if(mSocket->state()==QAbstractSocket::ConnectedState)
	{
		if(!((QSslSocket*)mSocket)->isEncrypted())
			((QSslSocket*)mSocket)->startServerEncryption();
		else onConnected();
	}
	resetIdentification(newId,newName);
	streamParser.reset();
	onDeviceReset();
}

void TcpSslDevice::setNewSocket(QSslSocket *s,const QUuid &newId,const QByteArray &newName)
{
	mSocket->disconnectFromHost();
	delete mSocket;
	mAddress.clear();
	mSocket=s;
	if(!mSocket)return;
	mSocket->setParent(this);
	readAddrFromSocket(s->socketDescriptor());
	setupSocket();
	if(mSocket->state()==QAbstractSocket::ConnectedState)
	{
		if(!((QSslSocket*)mSocket)->isEncrypted())
			((QSslSocket*)mSocket)->startServerEncryption();
		else onConnected();
	}
	resetIdentification(newId,newName);
	streamParser.reset();
	onDeviceReset();
}

bool TcpSslDevice::waitForConnected()
{
	return sslSocket()->waitForEncrypted();
}

QByteArray TcpSslDevice::deviceType()
{
	return "tcp+ssl";
}

void TcpSslDevice::onSocketEncrypted()
{
	onConnected();
	identify();
}

void TcpSslDevice::onSslErrors()
{
	mSocket->disconnectFromHost();
}

void TcpSslDevice::startSocketConnection()
{
	sslSocket()->connectToHostEncrypted(mAddress,WLIOTConfig::netDeviceSslPort);
}

void TcpSslDevice::processOnSocketConnected()
{
}
