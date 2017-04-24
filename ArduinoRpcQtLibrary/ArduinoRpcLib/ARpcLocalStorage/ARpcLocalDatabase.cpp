#include "ARpcLocalDatabase.h"

ARpcLocalDatabase::ARpcLocalDatabase(QObject *parent)
	:QObject(parent)
{
	opened=false;
}

bool ARpcLocalDatabase::open(const QString &path)
{
	opened=false;
	dbDir=QDir(path);
	if(!dbDir.exists())return false;
	opened=true;
	return true;
}

bool ARpcLocalDatabase::listSensors(QList<DeviceAndSensorId> &list)
{
	if(!opened)return false;
	list.clear();
	QStringList dirs=dbDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	QRegExp expr("^(\\{[0-9a-f]{8,8}-[0-9a-f]{4,4}-[0-9a-f]{4,4}-[0-9a-f]{4,4}-[0-9a-f]{12,12}\\})_(\\w+)$");
	for(QString &d:dirs)
	{
		if(expr.indexIn(d)==-1)continue;
		list.append({QUuid(expr.cap(1)),expr.cap(2)});
	}
	return true;
}

ARpcISensorStorage* ARpcLocalDatabase::preOpen(const DeviceAndSensorId &id)
{
	QString path=dbDir.absolutePath()+"/"+id.deviceId.toString()+"_"+id.sensorName;
	return ARpcISensorStorage::preOpen(path);
}

ARpcISensorStorage* ARpcLocalDatabase::preCreate(const DeviceAndSensorId &id,
	ARpcISensorStorage::StoreMode storeMode,ARpcSensor::Type sensorType)
{
	QString path=dbDir.absolutePath()+"/"+id.deviceId.toString()+"_"+id.sensorName;
	QFileInfo info(path);
	if(!info.exists())return 0;
	return ARpcISensorStorage::preCreate(path,storeMode,sensorType);
}
