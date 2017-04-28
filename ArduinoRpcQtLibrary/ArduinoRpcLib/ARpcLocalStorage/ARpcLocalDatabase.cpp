#include "ARpcLocalDatabase.h"

ARpcLocalDatabase::ARpcLocalDatabase(QObject *parent)
	:QObject(parent)
{
	opened=false;
}

bool ARpcLocalDatabase::open(const QString &path)
{
	close();
	dbDir=QDir(path);
	if(!dbDir.exists())return false;
	QStringList dirs=dbDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	QRegExp expr("^(\\{[0-9a-f]{8,8}-[0-9a-f]{4,4}-[0-9a-f]{4,4}-[0-9a-f]{4,4}-[0-9a-f]{12,12}\\})_(\\w+)$");
	for(QString &d:dirs)
	{
		if(expr.indexIn(d)==-1)continue;
		QString path=dbDir.absolutePath()+"/"+d;
		ARpcISensorStorage *st=ARpcISensorStorage::preOpen(path);
		if(!st)continue;
		storagesIds.append({QUuid(expr.cap(1)),expr.cap(2)});
		storages.append(st);
	}
	opened=true;
	return true;
}

void ARpcLocalDatabase::close()
{
	if(!opened)return;
	opened=false;
	for(auto v:storages)delete v;
	storages.clear();
	storagesIds.clear();
}

bool ARpcLocalDatabase::listSensors(QList<DeviceAndSensorId> &list)
{
	if(!opened)return false;
	list=storagesIds;
	return true;
}

ARpcISensorStorage* ARpcLocalDatabase::existingStorage(const DeviceAndSensorId &id)
{
	if(!opened)return 0;
	int index=storagesIds.indexOf(id);
	if(index==-1)return 0;
	return storages[index];
}

ARpcISensorStorage* ARpcLocalDatabase::preCreate(const DeviceAndSensorId &id,
	ARpcISensorStorage::StoreMode storeMode,ARpcSensor::Type sensorType)
{
	if(!opened)return 0;
	if(id.deviceId.isNull()||id.sensorName.isEmpty()||storagesIds.contains(id))return 0;
	QString path=dbDir.absolutePath()+"/"+id.deviceId.toString()+"_"+id.sensorName;
	QFileInfo info(path);
	if(info.exists())return 0;
	auto retVal=ARpcISensorStorage::preCreate(path,storeMode,sensorType);
	if(retVal)
	{
		storagesIds.append(id);
		storages.append(retVal);
	}
	return retVal;
}
