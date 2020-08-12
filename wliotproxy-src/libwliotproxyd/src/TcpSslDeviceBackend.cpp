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

using namespace WLIOT;

const QByteArray TcpSslDeviceBackend::mBackendType=QByteArray("tcps");

TcpSslDeviceBackend::TcpSslDeviceBackend(const QString &addr,const QString &connString,QObject *parent)
	:TcpDeviceBackend(addr,connString,WLIOTProtocolDefs::netDeviceSslPort,parent)
{
	mSocket=new QSslSocket(this);
	setupSocket();

	if(!mAddress.isNull())
		startSocketConnection();
}

TcpSslDeviceBackend::TcpSslDeviceBackend(qintptr s,const QString &addr,quint16 port,QObject *parent)
	:TcpDeviceBackend(addr,"",port,parent)
{
	mSocket=new QSslSocket(this);
	mSocket->setSocketDescriptor(s);
	setupSocket();

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

QByteArray TcpSslDeviceBackend::backendType() const
{
	return mBackendType;
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

void TcpSslDeviceBackend::setupSocket()
{
	TcpDeviceBackend::setupSocket();
	((QSslSocket*)mSocket)->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
	((QSslSocket*)mSocket)->setPeerVerifyMode(QSslSocket::VerifyNone);
	connect(sslSocket(),&QSslSocket::encrypted,this,&TcpSslDeviceBackend::onSocketEncrypted,Qt::QueuedConnection);
	connect(sslSocket(),static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&TcpSslDeviceBackend::onSslErrors);
}
