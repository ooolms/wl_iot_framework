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
#include "ARpcAllStorages.h"

static const int metaTypeIdForDeviceAndSensorId=qMetaTypeId<DeviceStorageId>();

ARpcLocalDatabase::ARpcLocalDatabase(QObject *parent)
	:QObject(parent)
{
	mOpened=false;
}

bool ARpcLocalDatabase::open(const QString &path)
{
	close();
	dbDir=QDir(path);
	if(!dbDir.exists())
		return false;
	QStringList dirs=dbDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString &d:dirs)
	{
		QString path=dbDir.absolutePath()+"/"+d;
		ARpcISensorStorage *st=ARpcISensorStorage::preOpen(path);
		if(!st)
			continue;
		DeviceStorageId id={st->deviceId(),st->sensor().name};
		if(storages.contains(id))
		{
			delete st;
			continue;
		}
		else storages[id]=st;
	}
	mOpened=true;
	emit opened();
	return true;
}

void ARpcLocalDatabase::close()
{
	if(!mOpened)
		return;
	mOpened=false;
	emit closed();
	for(auto v:storages)
		delete v;
	storages.clear();
}

bool ARpcLocalDatabase::isOpened()
{
	return mOpened;
}

bool ARpcLocalDatabase::listSensors(QList<DeviceStorageId> &list)
{
	if(!mOpened)
		return false;
	list=storages.keys();
	return true;
}

bool ARpcLocalDatabase::listSensorsWithDevNames(QList<DeviceStorageId> &list,QByteArrayList &titles)
{
	if(!mOpened)
		return false;
	list.clear();
	titles.clear();
	for(auto i=storages.begin();i!=storages.end();++i)
	{
		list.append(i.key());
		titles.append(i.value()->deviceName());
	}
	return true;
}

ARpcISensorStorage* ARpcLocalDatabase::existingStorage(const DeviceStorageId &id)
{
	if(!mOpened)
		return 0;
	if(!storages.contains(id))
		return 0;
	return storages[id];
}

ARpcISensorStorage* ARpcLocalDatabase::preCreate(const QUuid &devId,const QByteArray &devName,
	ARpcISensorStorage::StoreMode storeMode,const ARpcSensorDef &sensor,
	ARpcISensorStorage::TimestampRule rule)
{
	if(!mOpened)
		return 0;
	DeviceStorageId id={devId,sensor.name};
	if(devId.isNull()||sensor.name.isEmpty()||storages.contains(id))
		return 0;
	QString path=dbDir.absolutePath()+"/"+devId.toString()+"_"+QString::fromUtf8(sensor.name);
	QFileInfo info(path);
	if(info.exists())
		return 0;
	ARpcISensorStorage *retVal=ARpcISensorStorage::preCreate(path,storeMode,sensor,devId,devName,rule);
	if(retVal)
		storages[id]=retVal;
	return retVal;
}

ARpcISensorStorage* ARpcLocalDatabase::create(const QUuid &devId,const QByteArray &devName,
	ARpcISensorStorage::StoreMode mode,const ARpcSensorDef &sensor,
	ARpcISensorStorage::TimestampRule rule,int valuesCount,bool gtIndex)
{
	ARpcISensorStorage *stor=preCreate(devId,devName,mode,sensor,rule);
	if(!stor)
		return 0;
	quint32 dims=1;
	if(sensor.attributes.contains("dims"))
		dims=sensor.attributes["dims"].toUInt();
	if(dims==0)
		dims=1;
	bool ok=false;
	if(mode==ARpcISensorStorage::CONTINUOUS)
		ok=((ARpcContinuousStorage*)stor)->create(gtIndex);
	else if(mode==ARpcISensorStorage::AUTO_SESSIONS||mode==ARpcISensorStorage::MANUAL_SESSIONS)
		ok=((ARpcSessionStorage*)stor)->create(gtIndex);
	else if(mode==ARpcISensorStorage::LAST_N_VALUES)
		ok=((ARpcLastNValuesStorage*)stor)->create(valuesCount);
	else if(mode==ARpcISensorStorage::LAST_N_VALUES_IN_MEMORY)
		ok=((ARpcLastNValuesInMemoryStorage*)stor)->create(valuesCount);
	if(!ok)
	{
		removeStorage({devId,sensor.name});
		return 0;
	}
	creationFinished({devId,sensor.name});
	return stor;
}

bool ARpcLocalDatabase::hasStorage(const DeviceStorageId &id)
{
	return storages.contains(id);
}

bool ARpcLocalDatabase::removeStorage(const DeviceStorageId &id)
{
	if(!mOpened)
		return false;
	if(!storages.contains(id))
		return false;
	if(storages[id]->isOpened())
		storages[id]->close();
	QString path=dbDir.absolutePath()+"/"+id.deviceId.toString()+"_"+id.sensorName;
	QDir dir(path);
	if(!rmDirRec(dir))
		return false;
	emit storageRemoved(id);
	delete storages[id];
	storages.remove(id);
	return true;
}

void ARpcLocalDatabase::creationFinished(const DeviceStorageId &id)
{
	if(!storages.contains(id))
		return;
	emit storageCreated(id);
}

ARpcISensorStorage* ARpcLocalDatabase::findStorageForDevice(
	const QByteArray &devIdOrName,const QByteArray &sensorName,QUuid &devId)
{
	for(auto i=storages.begin();i!=storages.end();++i)
	{
		if(i.key().sensorName!=sensorName)
			continue;
		if(i.key().deviceId.toByteArray()==devIdOrName)
		{
			devId=i.key().deviceId;
			return i.value();
		}
		else
		{
			QByteArray devNameFromStorage=i.value()->deviceName();
			if(!devNameFromStorage.isEmpty()&&devNameFromStorage==devIdOrName)
			{
				devId=i.key().deviceId;
				return i.value();
			}
		}
	}
	return 0;
}

QUuid ARpcLocalDatabase::findDeviceId(const QByteArray &devIdOrName)
{
	for(auto i=storages.begin();i!=storages.end();++i)
	{
		if(i.key().deviceId.toByteArray()==devIdOrName)
			return i.key().deviceId;
		else
		{
			QByteArray devNameFromStorage=i.value()->deviceName();
			if(!devNameFromStorage.isEmpty()&&devNameFromStorage==devIdOrName)
				return i.key().deviceId;
		}
	}
	return QUuid();
}

bool ARpcLocalDatabase::rmDirRec(QDir dir)
{
	QStringList entries=dir.entryList(QDir::Files|QDir::Hidden|QDir::System);
	for(int i=0;i<entries.count();++i)
		if(!dir.remove(entries[i]))
			return false;
	entries=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(int i=0;i<entries.count();++i)
	{
		QDir chDir=dir;
		if(!chDir.cd(entries[i]))
			return false;
		if(!rmDirRec(chDir))
			return false;
	}
	return dir.rmdir(dir.absolutePath());
}
