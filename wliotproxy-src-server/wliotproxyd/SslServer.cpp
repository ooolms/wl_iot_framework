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

#include "SslServer.h"
#include <QSslSocket>

SslServer::SslServer(QObject *parent)
	:QTcpServer(parent)
{
}

void SslServer::setSslOptions(const QList<QSslCertificate> &c,const QSslKey &k)
{
	crtChain=c;
	key=k;
}

void SslServer::incomingConnection(qintptr socketDescriptor)
{
	QSslSocket *serverSocket=new QSslSocket(this);
	if(serverSocket->setSocketDescriptor(socketDescriptor))
	{
		addPendingConnection(serverSocket);
		serverSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
		serverSocket->setPrivateKey(key);
		serverSocket->setLocalCertificateChain(crtChain);
		serverSocket->ignoreSslErrors(QList<QSslError>()<<QSslError::SelfSignedCertificate);
		serverSocket->startServerEncryption();
	}
	else
		delete serverSocket;
}
