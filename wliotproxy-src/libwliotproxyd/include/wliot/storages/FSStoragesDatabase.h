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

#ifndef FSSTORAGESDATABASE_H
#define FSSTORAGESDATABASE_H

#include <QObject>
#include <QDir>
#include <QUuid>
#include "wliot/devices/SensorDef.h"
#include "wliot/storages/BaseFSSensorStorage.h"
#include "wliot/storages/StorageId.h"
#include "wliot/storages/IStoragesDatabase.h"

class FSStoragesDatabase
	:public IStoragesDatabase
{
	Q_OBJECT

public:
	explicit FSStoragesDatabase(QObject *parent=0);
	bool open(const QString &path);
	void close();
	bool isOpened();
	virtual bool listStorages(QList<StorageId> &list)override;
	virtual bool listStoragesWithDevNames(QList<StorageId> &list,QByteArrayList &titles)override;
	virtual ISensorStorage* existingStorage(const StorageId &id)override;
	virtual ISensorStorage* create(const QUuid &devId,const QByteArray &devName,
		ISensorStorage::StoreMode mode,const SensorDef &sensor,ISensorStorage::TimestampRule rule,
		int valuesCount=1,bool gtIndex=false)override;
	virtual bool hasStorage(const StorageId &id)override;
	virtual bool removeStorage(const StorageId &id)override;
	virtual ISensorStorage* findStorageForDevice(
		const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &devId)override;
	virtual QUuid findDeviceId(const QByteArray &devIdOrName)override;

private:
	ISensorStorage* preCreate(const QUuid &devId,const QByteArray &devName,
		ISensorStorage::StoreMode storeMode,const SensorDef &sensor,
		ISensorStorage::TimestampRule rule);

private:
	QDir dbDir;
	bool mOpened;
	QMap<StorageId,ISensorStorage*> storages;
};

#endif // ARPCFSSTORAGESDATABASE_H
