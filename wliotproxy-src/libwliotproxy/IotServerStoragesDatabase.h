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

#ifndef IOTSERVERSTORAGESDATABASE_H
#define IOTSERVERSTORAGESDATABASE_H

#include "IotServerTypes.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"
#include "wliot/storages/IStoragesDatabase.h"
#include "IotServerStorage.h"
#include <QByteArrayList>

class IotServerStoragesDatabase
	:public IStoragesDatabase
{
	Q_OBJECT
public:
	explicit IotServerStoragesDatabase(IotServerConnection *conn,IotServerCommands *cmds,QObject *parent=0);
	virtual bool listStorages(QList<StorageId> &list)override;
	virtual bool listStoragesWithDevNames(QList<StorageId> &list,QByteArrayList &titles)override;
	virtual ISensorStorage *existingStorage(const StorageId &id)override;
	virtual ISensorStorage* create(const QUuid &devId,const QByteArray &devName,ISensorStorage::StoreMode mode,
		const SensorDef &sensor,ISensorStorage::TimestampRule rule,int valuesCount=1,
		bool gtIndex=false)override;
	virtual bool hasStorage(const StorageId &id)override;
	virtual bool removeStorage(const StorageId &id)override;
	virtual QUuid findDeviceId(const QByteArray &devIdOrName)override;
	virtual ISensorStorage *findStorageForDevice(
		const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &devId)override;

private slots:
	void onStorageCreatedFromServer(const IotServerStorageDescr &s);
	void onStorageRemovedFromServer(const StorageId &id);
	void onServerConnected();
	void onServerDisconnected();
	void onNewValue(const StorageId &id,const QByteArrayList &args);

private:
	ISensorStorage* createWrap(const IotServerStorageDescr &s);

private:
	IotServerConnection *srvConn;
	IotServerCommands *commands;
	QMap<StorageId,ISensorStorage*> storages;
};

#endif // IOTSERVERSTORAGESDATABASE_H
