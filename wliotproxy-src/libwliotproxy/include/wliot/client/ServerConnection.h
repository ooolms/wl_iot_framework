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

#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <QObject>
#include <QLocalSocket>
#include <QSslSocket>
#include <QByteArrayList>
#include <QNetworkProxy>
#include <QTimer>
#include <QThread>
#include "wliot/WLIOTServerProtocolDefs.h"
#include "wliot/client/ServerDataTypes.h"
#include "wliot/devices/StreamParser.h"
#include "wliot/devices/SensorValue.h"
#include "wliot/storages/StorageId.h"

namespace WLIOTClient
{
	class ServerConnectionSocketWrap;

	class ServerConnection
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ServerConnection(QObject *parent=nullptr);
		virtual ~ServerConnection();
		bool startConnectLocal();
		void setNoDebug(bool n);
		bool startConnectNet(const QString &host,quint16 port=WLIOT::WLIOTServerProtocolDefs::controlSslPort);
		bool authenticateNet(const QByteArray &userName,const QByteArray &pass);
		bool authenticateLocalFromRoot(const QByteArray &userName);
		bool isConnected();
		bool execCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal,CmDataCallback onCmData=0);
		bool execCommand(const QByteArray &cmd,const QByteArrayList &args,CmDataCallback onCmData=0);
		bool waitForConnected(int msec=30000);
		void disconnectFromServer();
		bool subscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName);
		bool unsubscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName);
		bool identifyServer(QUuid &id,QByteArray &name);
		bool writeMsg(const WLIOT::Message &m);
		void setProxy(const QNetworkProxy &p);
		bool writeVDevMsg(const QUuid &id,const WLIOT::Message &m);
		ApmIdType userId();//-1 if not authenticated or unknown user

	signals:
		void preconnected();
		void connected();
		void disconnected();
		void needAuthentication();
		void connectionError();
		void newSensorValue(const WLIOT::StorageId &id,const QByteArrayList &valueArgs);
		void deviceIdentified(const QUuid &id,const QByteArray &name,const QUuid &typeId);
		void deviceLost(const QUuid &id);
		void deviceStateChanged(const QUuid &id,const QByteArrayList &args);
		void storageCreated(const WLIOTClient::StorageDescr &s);
		void storageRemoved(const WLIOT::StorageId &id);
		void funcCallReplyMsg(const WLIOT::Message &m);
		void vdevMsg(const QUuid &id,const WLIOT::Message &m);
		void authenticationChanged();

	private slots:
		void onLocalSocketConnected();
		void onNetDeviceConnected();
		void onDevDisconnected();
		void onRawMessage(const WLIOT::Message &m);
		void onSyncTimer();
		void onNewData(QByteArray data);

	private:
		friend class ServerCommandCall;
		friend class ServerConnectionSocketWrap;

		WLIOT::StreamParser parser;
		QThread *sockThread;
		ServerConnectionSocketWrap *sock;
		bool netConn;
		bool ready;
		quint64 callIdNum;
		QNetworkProxy proxy;
		ApmIdType uid;
		QTimer syncTimer;
		QString mHost;
		quint16 mPort;
		bool wasSyncMsg;
		bool noDebug;
	};
}

#endif // SERVERCONNECTION_H