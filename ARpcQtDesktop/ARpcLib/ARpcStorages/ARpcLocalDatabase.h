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

#ifndef ARPCLOCALDATABASE_H
#define ARPCLOCALDATABASE_H

#include <QObject>
#include <QDir>
#include <QUuid>
#include "ARpcBase/ARpcSensorDef.h"
#include "ARpcStorages/ARpcBaseFSSensorStorage.h"
#include "ARpcStorages/ARpcStorageId.h"

/**
 * @brief The ARpcLocalDatabase class
 * База с базами данных датчиков. Задача класса - управлять базами со значениями отдельных датчиков
 * (создание/удаление/перечисление).
 */
class ARpcLocalDatabase
	:public QObject
{
	Q_OBJECT

public:
	explicit ARpcLocalDatabase(QObject *parent=0);
	bool open(const QString &path);
	void close();
	bool isOpened();
	bool listSensors(QList<ARpcStorageId> &list);
	bool listSensorsWithDevNames(QList<ARpcStorageId> &list,QByteArrayList &titles);
	ARpcBaseFSSensorStorage* existingStorage(const ARpcStorageId &id);
	ARpcBaseFSSensorStorage* create(const QUuid &devId,const QByteArray &devName,ARpcISensorStorage::StoreMode mode,
		const ARpcSensorDef &sensor,ARpcISensorStorage::TimestampRule rule,int valuesCount=1,bool gtIndex=false);
	bool hasStorage(const ARpcStorageId &id);
	bool removeStorage(const ARpcStorageId &id);
	void creationFinished(const ARpcStorageId &id);
	ARpcBaseFSSensorStorage* findStorageForDevice(
		const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &devId);
	QUuid findDeviceId(const QByteArray &devIdOrName);

signals:
	void opened();
	void closed();
	void storageCreated(const ARpcStorageId &id);
	void storageRemoved(const ARpcStorageId &id);

private:
	ARpcBaseFSSensorStorage* preCreate(const QUuid &devId,const QByteArray &devName,
		ARpcISensorStorage::StoreMode storeMode,const ARpcSensorDef &sensor,
		ARpcISensorStorage::TimestampRule rule);

private:
	QDir dbDir;
	bool mOpened;
	QMap<ARpcStorageId,ARpcBaseFSSensorStorage*> storages;
};

#endif // ARPCLOCALDATABASE_H
