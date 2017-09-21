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
#include "ARpcBase/ARpcSensor.h"
#include "ARpcLocalStorage/ARpcISensorStorage.h"

struct DeviceAndSensorId
{
	QUuid deviceId;
	QString sensorName;
	bool operator==(const DeviceAndSensorId &id) const
	{
		return deviceId==id.deviceId&&sensorName==id.sensorName;
	}

	bool operator<(const DeviceAndSensorId &id) const
	{
		if(deviceId==id.deviceId)
			return sensorName<id.sensorName;
		return deviceId<id.deviceId;
	}
};

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
	bool listSensors(QList<DeviceAndSensorId> &list);
	bool listSensorsWithDevNames(QList<DeviceAndSensorId> &list,QStringList &titles);
	ARpcISensorStorage* existingStorage(const DeviceAndSensorId &id);
	ARpcISensorStorage* create(const DeviceAndSensorId &id,ARpcISensorStorage::StoreMode mode,
		const ARpcSensor &sensor,ARpcISensorStorage::TimestampRule rule,int nForLastNValues=1);
	bool hasStorage(const DeviceAndSensorId &id);
	bool removeStorage(const DeviceAndSensorId &id);
	void creationFinished(const DeviceAndSensorId &id);
	ARpcISensorStorage* findStorageForDevice(const QString &devIdOrName,const QString &sensorName,QUuid &devId);

signals:
	void opened();
	void closed();
	void storageCreated(const DeviceAndSensorId &id);
	void storageRemoved(const DeviceAndSensorId &id);

private:
	static bool rmDirRec(QDir dir);
	ARpcISensorStorage* preCreate(const DeviceAndSensorId &id,ARpcISensorStorage::StoreMode storeMode,
		ARpcSensor::Type sensorType,ARpcISensorStorage::TimestampRule rule);

private:
	QDir dbDir;
	bool mOpened;
	QList<DeviceAndSensorId> storagesIds;
	QList<ARpcISensorStorage*> storages;
	//TODO replace 2 QList with 1 QMap ?
};

#endif // ARPCLOCALDATABASE_H
