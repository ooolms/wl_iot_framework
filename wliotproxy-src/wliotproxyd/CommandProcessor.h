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
#include "wliot/devices/VirtualDevice.h"
#include "Commands/ICommand.h"
#include "wliot/storages/FSStoragesDatabase.h"
#include "AccessManagement/AccessMgr.h"
#include <QObject>
#include <QMap>
#include <QIODevice>

class QLocalSocket;
class QSslSocket;

class CommandProcessor
	:public QObject
{
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
	virtual ~CommandProcessor();
	void scheduleDelete();
	void registerVDevForCommandsProcessing(VirtualDevice *d);
	IdType uid();
	void writeMsg(const Message &m);
	void writeMsg(const QByteArray &msg,const QByteArrayList &args=QByteArrayList());

public slots:
	void onNewValueWritten(const SensorValue *value);

signals:
	void syncFailed();

private slots:
	void onNewMessage(const Message &m);
	void onDeviceIdentified(QUuid id,QByteArray name,QUuid typeId);
	void onDeviceStateChanged(QUuid id,QByteArrayList args);
	void onDeviceLost(QUuid id);
	void onStorageCreated(const StorageId &id);
	void onStorageRemoved(const StorageId &id);
	void onMessageToVDev(const Message &m);
	void onVDevDestroyed();
	void onSyncTimer();
	void onReadyRead();
	void onNewData(QByteArray data);

private:
	void construct();
	void addCommand(ICommand *c);
	void writeData(const QByteArray &d);

private:
	union
	{
		QLocalSocket *localSock;
		QSslSocket *netSock;
	};
	StreamParser parser;
	QTimer syncTimer;
	QMap<QString,ICommand*> commandProcs;
	QList<ICommand*> commands;
	QMap<QUuid,VirtualDevice*> vDevs;
	qint32 mUid;
	int inWorkCommands;
	bool needDeleteThis;
	bool mWasSync;
	bool localClient;
	int cliNum;
};

#endif // COMMANDPROCESSOR_H
