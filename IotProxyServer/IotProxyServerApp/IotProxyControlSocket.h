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

#ifndef IOTPROXYCONTROLSOCKET_H
#define IOTPROXYCONTROLSOCKET_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include "ARpcBase/ARpcOutsideDevice.h"
#include "IotProxyCommandProcessor.h"
#include "ClientThread.h"
#include "ARpcStorages/ARpcFSStoragesDatabase.h"

class IotProxyControlSocket
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

		QLocalSocket *sock;
		ARpcOutsideDevice *dev;
		IotProxyCommandProcessor *proc;
	};

public:
	explicit IotProxyControlSocket(QObject *parent=0);
	virtual ~IotProxyControlSocket();
	void start();

private slots:
	void onNewLocalConnection();
	void onLocalSocketDisconnected();

private:
	int findClient(QLocalSocket *sock);

private:
	QLocalServer localServer;
	QList<ClientSet> clients;
//	QList<ClientThread*> clients;
};

#endif // IOTPROXYCONTROLSOCKET_H
