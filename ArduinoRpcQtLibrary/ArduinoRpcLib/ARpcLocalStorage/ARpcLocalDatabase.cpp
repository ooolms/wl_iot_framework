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

bool ARpcLocalDatabase::listSensorsWithDevNames(QList<DeviceAndSensorId> &list,QStringList &titles)
{
	if(!opened)return false;
	list.clear();
	titles.clear();
	for(int i=0;i<storagesIds.count();++i)
	{
		list.append(storagesIds[i]);
		titles.append(storages[i]->getDeviceName());
	}
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

bool ARpcLocalDatabase::hasStorage(const DeviceAndSensorId &id)
{
	return storagesIds.contains(id);
}

bool ARpcLocalDatabase::removeStorage(const DeviceAndSensorId &id)
{
	if(!opened)return false;
	int index=storagesIds.indexOf(id);
	if(index==-1)return false;
	if(storages[index]->isOpened())storages[index]->close();
	QString path=dbDir.absolutePath()+"/"+id.deviceId.toString()+"_"+id.sensorName;
	QDir dir(path);
	if(!rmDirRec(dir))return false;
	delete storages[index];
	storages.removeAt(index);
	storagesIds.removeAt(index);
	return true;
}

bool ARpcLocalDatabase::rmDirRec(QDir dir)
{
	QStringList entries=dir.entryList(QDir::Files|QDir::Hidden|QDir::System);
	for(int i=0;i<entries.count();++i)if(!dir.remove(entries[i]))return false;
	entries=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(int i=0;i<entries.count();++i)
	{
		QDir chDir=dir;
		if(!chDir.cd(entries[i]))return false;
		if(!rmDirRec(chDir))return false;
	}
	return dir.rmdir(dir.absolutePath());
}
