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
	bool operator==(const DeviceAndSensorId &id)const
	{
		return deviceId==id.deviceId&&sensorName==id.sensorName;
	}
	bool operator<(const DeviceAndSensorId &id)const
	{
		if(deviceId==id.deviceId)return sensorName<id.sensorName;
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
	virtual bool listSensors(QList<DeviceAndSensorId> &list);
	virtual ARpcISensorStorage* existingStorage(const DeviceAndSensorId &id);
	virtual ARpcISensorStorage* preCreate(const DeviceAndSensorId &id,
		ARpcISensorStorage::StoreMode storeMode,ARpcSensor::Type sensorType);
	virtual bool hasStorage(const DeviceAndSensorId &id);
	virtual bool removeStorage(const DeviceAndSensorId &id);

private:
	static bool rmDirRec(QDir dir);

private:
	QDir dbDir;
	bool opened;
	QList<DeviceAndSensorId> storagesIds;
	QList<ARpcISensorStorage*> storages;
};

#endif // ARPCLOCALDATABASE_H
