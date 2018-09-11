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

#ifndef ARPCFSSTORAGESDATABASE_H
#define ARPCFSSTORAGESDATABASE_H

#include <QObject>
#include <QDir>
#include <QUuid>
#include "ARpcBase/ARpcSensorDef.h"
#include "ARpcStorages/ARpcBaseFSSensorStorage.h"
#include "ARpcStorages/ARpcStorageId.h"
#include "ARpcStorages/ARpcIStoragesDatabase.h"

class ARpcFSStoragesDatabase
	:public ARpcIStoragesDatabase
{
	Q_OBJECT

public:
	explicit ARpcFSStoragesDatabase(QObject *parent=0);
	bool open(const QString &path);
	void close();
	bool isOpened();
	virtual bool listStorages(QList<ARpcStorageId> &list)override;
	virtual bool listStoragesWithDevNames(QList<ARpcStorageId> &list,QByteArrayList &titles)override;
	virtual ARpcISensorStorage* existingStorage(const ARpcStorageId &id)override;
	virtual ARpcISensorStorage* create(const QUuid &devId,const QByteArray &devName,
		ARpcISensorStorage::StoreMode mode,const ARpcSensorDef &sensor,ARpcISensorStorage::TimestampRule rule,
		int valuesCount=1,bool gtIndex=false)override;
	virtual bool hasStorage(const ARpcStorageId &id)override;
	virtual bool removeStorage(const ARpcStorageId &id)override;
	virtual ARpcISensorStorage* findStorageForDevice(
		const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &devId)override;
	virtual QUuid findDeviceId(const QByteArray &devIdOrName)override;

private:
	ARpcISensorStorage* preCreate(const QUuid &devId,const QByteArray &devName,
		ARpcISensorStorage::StoreMode storeMode,const ARpcSensorDef &sensor,
		ARpcISensorStorage::TimestampRule rule);

private:
	QDir dbDir;
	bool mOpened;
	QMap<ARpcStorageId,ARpcISensorStorage*> storages;
};

#endif // ARPCFSSTORAGESDATABASE_H
