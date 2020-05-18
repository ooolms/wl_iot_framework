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

#ifndef ISTORAGESDATABASE_H
#define ISTORAGESDATABASE_H

#include <QObject>
#include "wliot/storages/ISensorStorage.h"
#include "wliot/storages/StorageId.h"

class IStoragesDatabase
	:public QObject
{
	Q_OBJECT
public:
	explicit IStoragesDatabase(QObject *parent=nullptr);
	QList<StorageId> allStorages();
	virtual bool listStorages(QList<StorageId> &list)=0;
	virtual bool listStoragesWithDevNames(QList<StorageId> &list,QByteArrayList &titles)=0;
	virtual ISensorStorage* existingStorage(const StorageId &id)=0;
	virtual ISensorStorage* create(const QUuid &devId,const QByteArray &devName,ISensorStorage::StoreMode mode,
		const SensorDef &sensor,ISensorStorage::TimestampRule rule,int valuesCount=1,bool gtIndex=false)=0;
	virtual bool hasStorage(const StorageId &id)=0;
	virtual bool removeStorage(const StorageId &id)=0;
	virtual QUuid findDeviceId(const QByteArray &devIdOrName)=0;
	virtual ISensorStorage* findStorageForDevice(
		const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &devId)=0;
	virtual QByteArray findDevName(const QUuid &devId)=0;

signals:
	void opened();
	void closed();
	void storageCreated(const StorageId &id);
	void storageRemoved(const StorageId &id);
};

#endif // ISTORAGESDATABASE_H
