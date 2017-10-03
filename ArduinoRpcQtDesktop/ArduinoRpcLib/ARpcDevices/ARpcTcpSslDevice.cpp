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

ARpcTcpSslDevice::ARpcTcpSslDevice(const QHostAddress &addr,QObject *parent)
	:ARpcRealDevice(parent)
{
	mAddress=addr;
	socket.ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
	socket.setPeerVerifyMode(QSslSocket::VerifyNone);
	retryTimer.setInterval(60*1000);
	retryTimer.setSingleShot(false);

	connect(&retryTimer,&QTimer::timeout,this,&ARpcTcpSslDevice::onRetryTimer);
	connect(&socket,&QSslSocket::connected,this,&ARpcTcpSslDevice::onSocketConnected);
	connect(&socket,&QSslSocket::disconnected,this,&ARpcTcpSslDevice::onSocketDisonnected);
	connect(&socket,&QSslSocket::encrypted,this,&ARpcTcpSslDevice::onSocketEncrypted);
	connect(&socket,static_cast<void(QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
		this,&ARpcTcpSslDevice::onSslErrors);

	retryTimer.start();
	onRetryTimer();
}

bool ARpcTcpSslDevice::writeMsg(const ARpcMessage &m)
{
	if(!isConnected())return false;
	QByteArray data=(msgParser.dump(m)+ARpcConfig::msgDelim).toUtf8();
	if(socket.write(data)!=data.size())return false;
	return socket.flush();
}

bool ARpcTcpSslDevice::isConnected()
{
	return socket.state()==QAbstractSocket::ConnectedState&&socket.isEncrypted();
}

QHostAddress ARpcTcpSslDevice::address()const
{
	return mAddress;
}

void ARpcTcpSslDevice::onRetryTimer()
{
	socket.disconnectFromHost();
	socket.waitForDisconnected(1000);
	socket.connectToHost(mAddress,ARpcConfig::netDeviceSslPort);
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
	socket.disconnectFromHost();
	retryTimer.start();
}

void ARpcTcpSslDevice::onSocketDisonnected()
{
	emit disconnected();
	resetIdentification();
	retryTimer.start();
}
