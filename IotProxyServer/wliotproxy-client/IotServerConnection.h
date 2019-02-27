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
#include "ARpcBase/ARpcServerConfig.h"
#include "IotServerTypes.h"
#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcSensorValue.h"
#include "ARpcStorages/ARpcBaseFSSensorStorage.h"
#include "ARpcStorages/ARpcStorageId.h"

class IotServerConnection
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerConnection(QObject *parent=nullptr);
	bool startConnectLocal();
	bool startConnectNet(const QString &host,quint16 port=ARpcServerConfig::controlSslPort);
	bool authentificateNet(const QByteArray &userName,const QByteArray &pass);
	bool authentificateLocalFromRoot(const QByteArray &userName);
	bool isConnected();
	bool execCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal,CmDataCallback onCmData=0);
	bool execCommand(const QByteArray &cmd,const QByteArrayList &args,CmDataCallback onCmData=0);
	bool waitForConnected(int msec=30000);
	void disconnectFromServer();
	bool subscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName);
	bool unsubscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName);
	bool identifyServer(QUuid &id,QByteArray &name);
	bool writeMsg(const ARpcMessage &m);
	void setProxy(const QNetworkProxy &p);

signals:
	void connected();
	void disconnected();
	void needAuthentification();
	void connectionError();
	void newSensorValue(const ARpcStorageId &id,const QByteArrayList &valueArgs);
	void deviceIdentified(const QUuid &id,const QByteArray &name,const QByteArray &type);
	void deviceLost(const QUuid &id);
	void deviceStateChanged(const QUuid &id,const QByteArrayList &args);
	void storageCreated(const IotServerStorageDescr &s);
	void storageRemoved(const ARpcStorageId &id);
	void processVirtualDeviceCommand(
		const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal);
	void funcCallReplyMsg(const ARpcMessage &m);

private slots:
	void onNetDeviceConnected();
	void onLocalSocketError();
	void onNetError();
	void onSslError();
	void onDevDisconnected();
	void onRawMessage(const ARpcMessage &m);
	void onLocalReadyRead();
	void onNetReadyRead();

private:
	friend class IotServerCommandCall;
	union
	{
		QLocalSocket *localSock;
		QSslSocket *netSock;
	};
	ARpcStreamParser parser;
	bool netConn;
	bool netAuthentificated;
	quint64 callIdNum;
	QNetworkProxy proxy;
};

#endif // IOTSERVERCONNECTION_H
