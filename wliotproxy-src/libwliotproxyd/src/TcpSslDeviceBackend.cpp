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

#include "wliot/devices/TcpSslDeviceBackend.h"
#include "wliot/WLIOTProtocolDefs.h"
#include <QHostAddress>

const QByteArray TcpSslDeviceBackend::devType=QByteArray("tcps");

TcpSslDeviceBackend::TcpSslDeviceBackend(const QString &addr,QObject *parent)
	:TcpDeviceBackend(parent)
{
	mAddress=addr;
	mSocket=new QSslSocket(this);
	((QSslSocket*)mSocket)->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
	((QSslSocket*)mSocket)->setPeerVerifyMode(QSslSocket::VerifyNone);
	setupSocket();
	connect(sslSocket(),&QSslSocket::encrypted,this,&TcpSslDeviceBackend::onSocketEncrypted);
	connect(sslSocket(),static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&TcpSslDeviceBackend::onSslErrors);

	if(!mAddress.isNull())
		startSocketConnection();
}

TcpSslDeviceBackend::TcpSslDeviceBackend(qintptr s,QObject *parent)
	:TcpDeviceBackend(parent)
{
	mSocket=new QSslSocket(this);
	mSocket->setSocketDescriptor(s);
	((QSslSocket*)mSocket)->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
	((QSslSocket*)mSocket)->setPeerVerifyMode(QSslSocket::VerifyNone);
	readAddrFromSocket(s);
	setupSocket();
	connect(sslSocket(),&QSslSocket::encrypted,this,&TcpSslDeviceBackend::onSocketEncrypted,Qt::QueuedConnection);
	connect(sslSocket(),static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&TcpSslDeviceBackend::onSslErrors,Qt::DirectConnection);

	if(mSocket->state()==QAbstractSocket::ConnectedState)
		((QSslSocket*)mSocket)->startServerEncryption();
}

bool TcpSslDeviceBackend::waitForConnected(int msecs)
{
	return sslSocket()->waitForEncrypted(msecs);
}

bool TcpSslDeviceBackend::isConnected()const
{
	return sslSocket()->isEncrypted();
}

QByteArray TcpSslDeviceBackend::type() const
{
	return devType;
}

void TcpSslDeviceBackend::onSocketEncrypted()
{
	emit connected();
}

void TcpSslDeviceBackend::onSslErrors()
{
	mSocket->disconnectFromHost();
}

void TcpSslDeviceBackend::startSocketConnection()
{
	sslSocket()->connectToHostEncrypted(mAddress,WLIOTProtocolDefs::netDeviceSslPort);
}

void TcpSslDeviceBackend::processOnSocketConnected()
{
}
