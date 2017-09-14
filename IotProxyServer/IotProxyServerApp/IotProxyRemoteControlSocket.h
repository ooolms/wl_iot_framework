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

#ifndef IOTPROXYREMOTECONTROLSOCKET_H
#define IOTPROXYREMOTECONTROLSOCKET_H

#include <QObject>
#include <QSslSocket>
#include "QSslServer.h"
#include "ARpcBase/ARpcOutsideDevice.h"
#include "IotProxyCommandProcessor.h"
#include "ClientThread.h"

class IotProxyRemoteControlSocket
	:public QObject
{
	Q_OBJECT
	struct ClientSet
	{
		ClientSet()
		{
			sock=0;
			dev=0;
			proc=0;
		}

		QSslSocket *sock;
		ARpcOutsideDevice *dev;
		IotProxyCommandProcessor *proc;
	};

public:
	explicit IotProxyRemoteControlSocket(QObject *parent=0);
	virtual ~IotProxyRemoteControlSocket();
	void start(const QSslCertificate &crt,const QSslKey &key);
	void stop();

private slots:
	void onNewLocalConnection();
	void onSocketDisconnected();
	void onSocketEncrypted();
	void onSocketError(QAbstractSocket::SocketError);
	void onSslErrors(const QList<QSslError> &errs);

private:
	int findClient(QSslSocket *sock);
	void closeClient(QSslSocket *sock);

private:
	QSslServer sslServer;
	QList<ClientSet> clients;
//	QList<ClientThread*> clients;
};

#endif // IOTPROXYREMOTECONTROLSOCKET_H
