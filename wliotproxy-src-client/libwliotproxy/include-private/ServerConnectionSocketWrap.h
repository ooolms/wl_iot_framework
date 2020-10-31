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

#ifndef SERVERCONNECTIONSOCKETWRAP_H
#define SERVERCONNECTIONSOCKETWRAP_H

#include <QObject>

class QLocalSocket;
class QSslSocket;

namespace WLIOTClient
{
	class ServerConnection;

	class ServerConnectionSocketWrap
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ServerConnectionSocketWrap(ServerConnection *conn);
		Q_INVOKABLE void startConnectLocal();
		Q_INVOKABLE void startConnectNet();
		Q_INVOKABLE void disconnectFromServer();
		Q_INVOKABLE void writeData(QByteArray data);

	signals:
		void newData(QByteArray data);
		void connectionError();

	private slots:
		void onLocalReadyRead();
		void onNetReadyRead();
		void onLocalSocketError();
		void onNetError();
		void onSslError();

	public:
		union
		{
			QLocalSocket *localSock;
			QSslSocket *netSock;
		};
		ServerConnection *connection;
	};
}

#endif // SERVERCONNECTIONSOCKETWRAP_H
