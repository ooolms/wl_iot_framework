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

#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "wliot/devices/StreamParser.h"
#include "wliot/devices/VirtualDeviceBackend.h"
#include "Commands/ICommand.h"
#include "wliot/storages/FSStoragesDatabase.h"
#include "AccessManagement/AccessMgr.h"
#include <QObject>
#include <QTimer>
#include <QMap>
#include <QIODevice>

class QLocalSocket;
class QSslSocket;
class QProcess;

class CommandProcessor
	:public QObject
	,public WLIOT::IVirtualDeviceBackendCallback
{
	enum ConnType
	{
		UnixSock,
		TcpSock,
		ChildProc
	};

	class WorkLocker
	{
	public:
		explicit WorkLocker(CommandProcessor *p);
		~WorkLocker();

	private:
		CommandProcessor *proc;
	};

	Q_OBJECT
public:
	explicit CommandProcessor(QLocalSocket *s,QObject *parent=0);
	explicit CommandProcessor(QSslSocket *s,QObject *parent=0);
	explicit CommandProcessor(QProcess *p,QObject *parent=0);
	virtual ~CommandProcessor();
	void scheduleDelete();
	void registerVDev(WLIOT::VirtualDeviceBackend *d);
	bool hasVDev(const QUuid &id);
	void disconnectVDev(const QUuid &id);
	IdType uid();
	void writeMsg(const WLIOT::Message &m);
	void writeMsg(const QByteArray &msg,const QByteArrayList &args=QByteArrayList());
	virtual void onMessageToVDev(WLIOT::VirtualDeviceBackend *vDev,const WLIOT::Message &m)override;
	void subscribeOnStorage(WLIOT::ISensorStorage *st);
	void unsubscribeFromStorage(WLIOT::ISensorStorage *st);

signals:
	void syncFailed();

private slots:
	void onNewMessage(const WLIOT::Message &m);
	void onDeviceIdentified(QUuid id,QByteArray name,QUuid typeId);
	void onDeviceStateChanged(QUuid id,QByteArrayList args);
	void onDeviceLost(QUuid id);
	void onStorageCreated(const WLIOT::StorageId &id);
	void onStorageRemoved(const WLIOT::StorageId &id);
	void onVDevDestroyed();
	void onSyncTimer();
	void onReadyRead();
	void onNewData(QByteArray data);
	void onNewValueWritten(const WLIOT::SensorValue *value);

private:
	void construct();
	void addCommand(ICommand *c);
	void writeData(const QByteArray &d);
	void authenticate(const WLIOT::Message &m);

private:
	union
	{
		QLocalSocket *localSock;
		QSslSocket *netSock;
		QProcess *childProc;
	};
	WLIOT::StreamParser parser;
	QTimer syncTimer;
	QMap<QByteArray,ICommand*> commandProcs;
	QList<ICommand*> commands;
	QMap<QUuid,WLIOT::VirtualDeviceBackend*> vDevs;
	QMap<WLIOT::StorageId,QMetaObject::Connection> mSubscribedStorages;
	qint32 mUid;
	int inWorkCommands;
	bool needDeleteThis;
	ConnType connType;
	int cliNum;
	int mSyncCount;
};

#endif // COMMANDPROCESSOR_H
