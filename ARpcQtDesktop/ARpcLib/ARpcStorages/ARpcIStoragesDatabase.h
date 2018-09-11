#ifndef ARPCISTORAGESDATABASE_H
#define ARPCISTORAGESDATABASE_H

#include <QObject>
#include "ARpcStorages/ARpcISensorStorage.h"
#include "ARpcStorages/ARpcStorageId.h"

class ARpcIStoragesDatabase
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcIStoragesDatabase(QObject *parent=nullptr);
	virtual bool listStorages(QList<ARpcStorageId> &list)=0;
	virtual bool listStoragesWithDevNames(QList<ARpcStorageId> &list,QByteArrayList &titles)=0;
	virtual ARpcISensorStorage* existingStorage(const ARpcStorageId &id)=0;
	virtual ARpcISensorStorage* create(const QUuid &devId,const QByteArray &devName,ARpcISensorStorage::StoreMode mode,
		const ARpcSensorDef &sensor,ARpcISensorStorage::TimestampRule rule,int valuesCount=1,bool gtIndex=false)=0;
	virtual bool hasStorage(const ARpcStorageId &id)=0;
	virtual bool removeStorage(const ARpcStorageId &id)=0;
	virtual QUuid findDeviceId(const QByteArray &devIdOrName)=0;
	virtual ARpcISensorStorage* findStorageForDevice(
		const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &devId)=0;

signals:
	void opened();
	void closed();
	void storageCreated(const ARpcStorageId &id);
	void storageRemoved(const ARpcStorageId &id);
};

#endif // ARPCISTORAGESDATABASE_H
