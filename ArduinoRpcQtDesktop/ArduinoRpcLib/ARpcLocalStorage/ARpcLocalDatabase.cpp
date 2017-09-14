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

#include "ARpcLocalDatabase.h"

ARpcLocalDatabase::ARpcLocalDatabase(QObject *parent)
	:QObject(parent)
{
	mOpened=false;
}

bool ARpcLocalDatabase::open(const QString &path)
{
	close();
	dbDir=QDir(path);
	if(!dbDir.exists())return false;
	QStringList dirs=dbDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	QRegExp expr("^(\\{[0-9a-fA-F]{8,8}-[0-9a-fA-F]{4,4}-[0-9a-fA-F]{4,4}-[0-9a-fA-F]{4,4}-"
		"[0-9a-fA-F]{12,12}\\})_(\\w+)$");
	for(QString &d:dirs)
	{
		if(expr.indexIn(d)==-1)continue;
		QString path=dbDir.absolutePath()+"/"+d;
		ARpcISensorStorage *st=ARpcISensorStorage::preOpen(path);
		if(!st)continue;
		storagesIds.append({QUuid(expr.cap(1)),expr.cap(2)});
		storages.append(st);
	}
	mOpened=true;
	emit opened();
	return true;
}

void ARpcLocalDatabase::close()
{
	if(!mOpened)return;
	mOpened=false;
	emit closed();
	for(auto v:storages)delete v;
	storages.clear();
	storagesIds.clear();
}

bool ARpcLocalDatabase::isOpened()
{
	return mOpened;
}

bool ARpcLocalDatabase::listSensors(QList<DeviceAndSensorId> &list)
{
	if(!mOpened)return false;
	list=storagesIds;
	return true;
}

bool ARpcLocalDatabase::listSensorsWithDevNames(QList<DeviceAndSensorId> &list,QStringList &titles)
{
	if(!mOpened)return false;
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
	if(!mOpened)return 0;
	int index=storagesIds.indexOf(id);
	if(index==-1)return 0;
	return storages[index];
}

ARpcISensorStorage* ARpcLocalDatabase::preCreate(const DeviceAndSensorId &id,
	ARpcISensorStorage::StoreMode storeMode,ARpcSensor::Type sensorType,ARpcISensorStorage::TimestampRule rule)
{
	if(!mOpened)return 0;
	if(id.deviceId.isNull()||id.sensorName.isEmpty()||storagesIds.contains(id))return 0;
	QString path=dbDir.absolutePath()+"/"+id.deviceId.toString()+"_"+id.sensorName;
	QFileInfo info(path);
	if(info.exists())return 0;
	auto retVal=ARpcISensorStorage::preCreate(path,storeMode,sensorType,rule);
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
	if(!mOpened)return false;
	int index=storagesIds.indexOf(id);
	if(index==-1)return false;
	if(storages[index]->isOpened())storages[index]->close();
	QString path=dbDir.absolutePath()+"/"+id.deviceId.toString()+"_"+id.sensorName;
	QDir dir(path);
	if(!rmDirRec(dir))return false;
	emit storageRemoved(id);
	delete storages[index];
	storages.removeAt(index);
	storagesIds.removeAt(index);
	return true;
}

void ARpcLocalDatabase::creationFinished(const DeviceAndSensorId &id)
{
	if(!storagesIds.contains(id))return;
	emit storageCreated(id);
}

ARpcISensorStorage* ARpcLocalDatabase::findStorageForDevice(
	const QString &devIdOrName,const QString &sensorName,QUuid &devId)
{
	for(int i=0;i<storagesIds.count();++i)
	{
		if(storagesIds[i].sensorName!=sensorName)continue;
		if(storagesIds[i].deviceId.toString()==devIdOrName)
		{
			devId=storagesIds[i].deviceId;
			return storages[i];
		}
		else
		{
			QString devNameI=storages[i]->getDeviceName();
			if(!devNameI.isEmpty()&&devNameI==devIdOrName)
			{
				devId=storagesIds[i].deviceId;
				return storages[i];
			}
		}
	}
	return 0;
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
