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

#ifndef IOTPROXYCOMMANDPROCESSOR_H
#define IOTPROXYCOMMANDPROCESSOR_H

#include "wliot/devices/QtIODeviceWrap.h"
#include "wliot/devices/VirtualDevice.h"
#include "Commands/ICommand.h"
#include "wliot/storages/FSStoragesDatabase.h"
#include "AccessManagement/AccessMgr.h"
#include <QObject>
#include <QMap>

class IotProxyCommandProcessor
	:public QObject
{
	Q_OBJECT
public:
	explicit IotProxyCommandProcessor(QtIODeviceWrap *d,bool forceRoot,QObject *parent=0);
	virtual ~IotProxyCommandProcessor();
	void scheduleDelete();
	void registerVDevForCommandsProcessing(VirtualDevice *d);
	IdType uid();

public slots:
	void onNewValueWritten(const SensorValue *value);

private slots:
	void onNewMessage(const Message &m);
	void onDeviceIdentified(QUuid id,QByteArray name,QByteArray type);
	void onDeviceStateChanged(QUuid id,QByteArrayList args);
	void onDeviceLost(QUuid id);
	void onStorageCreated(const StorageId &id);
	void onStorageRemoved(const StorageId &id);
	void onProcessCommandFromVDev(const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal);

private:
	void addCommand(ICommand *c);

private:
	static const QByteArray vDevOkMsg;
	static const QByteArray vDevErrMsg;
	QtIODeviceWrap *dev;
	QMap<QString,ICommand*> commandProcs;
	QList<ICommand*> commands;
	QList<VirtualDevice*> vDevs;
	qint32 mUid;
	bool inWork;
	bool needDeleteThis;
};

#endif // IOTPROXYCOMMANDPROCESSOR_H
