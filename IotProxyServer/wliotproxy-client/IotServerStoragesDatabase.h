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

#ifndef REMOTESTORAGES_H
#define REMOTESTORAGES_H

#include "IotServerTypes.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"
#include "ARpcStorages/ARpcIStoragesDatabase.h"
#include "IotServerStorage.h"
#include <QByteArrayList>

class IotServerStoragesDatabase
	:public ARpcIStoragesDatabase
{
	Q_OBJECT
public:
	explicit IotServerStoragesDatabase(IotServerConnection *conn,IotServerCommands *cmds,QObject *parent=0);
	virtual bool listStorages(QList<ARpcStorageId> &list)override;
	virtual bool listStoragesWithDevNames(QList<ARpcStorageId> &list,QByteArrayList &titles)override;
	virtual ARpcISensorStorage *existingStorage(const ARpcStorageId &id)override;
	virtual ARpcISensorStorage* create(const QUuid &devId,const QByteArray &devName,ARpcISensorStorage::StoreMode mode,
		const ARpcSensorDef &sensor,ARpcISensorStorage::TimestampRule rule,int valuesCount=1,
		bool gtIndex=false)override;
	virtual bool hasStorage(const ARpcStorageId &id)override;
	virtual bool removeStorage(const ARpcStorageId &id)override;
	virtual QUuid findDeviceId(const QByteArray &devIdOrName)override;
	virtual ARpcISensorStorage *findStorageForDevice(
		const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &devId)override;

signals:
	void storageCreated(const ARpcStorageId &id);
	void storageRemoved(const ARpcStorageId &id);

private slots:
	void onStorageCreatedFromServer(const IotServerStorageDescr &s);
	void onStorageRemovedFromServer(const ARpcStorageId &id);
	void onServerConnected();
	void onServerDisconnected();
	void onNewValue(const ARpcStorageId &id,const QByteArrayList &args);

private:
	ARpcISensorStorage* createWrap(const IotServerStorageDescr &s);

private:
	IotServerConnection *srvConn;
	IotServerCommands *commands;
	QMap<ARpcStorageId,ARpcISensorStorage*> storages;
};

#endif // REMOTESTORAGES_H
