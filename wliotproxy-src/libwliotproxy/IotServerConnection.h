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

#ifndef IOTSERVERCONNECTION_H
#define IOTSERVERCONNECTION_H

#include <QObject>
#include <QLocalSocket>
#include <QSslSocket>
#include <QByteArrayList>
#include <QNetworkProxy>
#include <QTimer>
#include <QThread>
#include "wliot/WLIOTServerProtocolDefs.h"
#include "IotServerTypes.h"
#include "wliot/devices/StreamParser.h"
#include "wliot/devices/SensorValue.h"
#include "wliot/storages/StorageId.h"
#include "IotServerConnectionSocketWrap.h"

class IotServerConnection
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerConnection(QObject *parent=nullptr);
	virtual ~IotServerConnection();
	bool startConnectLocal();
	void setNoDebug(bool n);
	bool startConnectNet(const QString &host,quint16 port=WLIOTServerProtocolDefs::controlSslPort);
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
	bool writeMsg(const Message &m);
	void setProxy(const QNetworkProxy &p);
	bool writeVDevMsg(const QUuid &id,const Message &m);
	IotServerApmIdType userId();//-1 if not authenticated or unknown user

signals:
	void preconnected();
	void connected();
	void disconnected();
	void needAuthentication();
	void connectionError();
	void newSensorValue(const StorageId &id,const QByteArrayList &valueArgs);
	void deviceIdentified(const QUuid &id,const QByteArray &name,const QUuid &typeId);
	void deviceLost(const QUuid &id);
	void deviceStateChanged(const QUuid &id,const QByteArrayList &args);
	void storageCreated(const IotServerStorageDescr &s);
	void storageRemoved(const StorageId &id);
	void funcCallReplyMsg(const Message &m);
	void vdevMsg(const QUuid &id,const Message &m);
	void authenticationChanged();

private slots:
	void onLocalSocketConnected();
	void onNetDeviceConnected();
	void onDevDisconnected();
	void onRawMessage(const Message &m);
	void onSyncTimer();
	void onNewData(QByteArray data);

private:
	friend class IotServerCommandCall;
	friend class IotServerConnectionSocketWrap;

	StreamParser parser;
	QThread sockThread;
	IotServerConnectionSocketWrap *sock;
	bool netConn;
	bool ready;
	quint64 callIdNum;
	QNetworkProxy proxy;
	IotServerApmIdType uid;
	QTimer syncTimer;
	QString mHost;
	quint16 mPort;
	bool wasSyncMsg;
	bool noDebug;
};

#endif // IOTSERVERCONNECTION_H
