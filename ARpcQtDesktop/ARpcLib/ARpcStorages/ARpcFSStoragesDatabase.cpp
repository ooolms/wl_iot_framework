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

#include "ARpcFSStoragesDatabase.h"
#include "ARpcAllStorages.h"

static bool rmDirRec(QDir dir)
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

ARpcFSStoragesDatabase::ARpcFSStoragesDatabase(QObject *parent)
	:ARpcIStoragesDatabase(parent)
{
	mOpened=false;
}

bool ARpcFSStoragesDatabase::open(const QString &path)
{
	close();
	dbDir=QDir(path);
	if(!dbDir.exists())
		return false;
	QStringList dirs=dbDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString &d:dirs)
	{
		QString path=dbDir.absolutePath()+"/"+d;
		ARpcISensorStorage *st=ARpcFSSensorStorageHelper::preOpen(path);
		if(!st)
			continue;
		ARpcStorageId id={st->deviceId(),st->sensor().name};
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

void ARpcFSStoragesDatabase::close()
{
	if(!mOpened)
		return;
	mOpened=false;
	emit closed();
	for(auto v:storages)
		delete v;
	storages.clear();
}

bool ARpcFSStoragesDatabase::isOpened()
{
	return mOpened;
}

bool ARpcFSStoragesDatabase::listStorages(QList<ARpcStorageId> &list)
{
	if(!mOpened)
		return false;
	list=storages.keys();
	return true;
}

bool ARpcFSStoragesDatabase::listStoragesWithDevNames(QList<ARpcStorageId> &list,QByteArrayList &titles)
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

ARpcISensorStorage* ARpcFSStoragesDatabase::existingStorage(const ARpcStorageId &id)
{
	if(!mOpened)
		return 0;
	if(!storages.contains(id))
		return 0;
	return storages[id];
}

ARpcISensorStorage* ARpcFSStoragesDatabase::preCreate(const QUuid &devId,const QByteArray &devName,
	ARpcISensorStorage::StoreMode storeMode,const ARpcSensorDef &sensor,
	ARpcISensorStorage::TimestampRule rule)
{
	if(!mOpened)
		return 0;
	ARpcStorageId id={devId,sensor.name};
	if(devId.isNull()||sensor.name.isEmpty()||storages.contains(id))
		return 0;
	QString path=dbDir.absolutePath()+"/"+devId.toString()+"_"+QString::fromUtf8(sensor.name);
	QFileInfo info(path);
	if(info.exists())
		return 0;
	ARpcISensorStorage *retVal=ARpcFSSensorStorageHelper::preCreate(
		path,devId,devName,sensor,storeMode,rule);
	if(retVal)
		storages[id]=retVal;
	return retVal;
}

ARpcISensorStorage* ARpcFSStoragesDatabase::create(const QUuid &devId,const QByteArray &devName,
	ARpcISensorStorage::StoreMode mode,const ARpcSensorDef &sensor,
	ARpcISensorStorage::TimestampRule rule,int valuesCount,bool gtIndex)
{
	ARpcStorageId id={devId,sensor.name};
	if(storages.contains(id))return 0;
	ARpcISensorStorage *stor=preCreate(devId,devName,mode,sensor,rule);
	if(!stor)
		return 0;
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
		delete stor;
		storages.remove(id);
		QString path=dbDir.absolutePath()+"/"+id.deviceId.toString()+"_"+id.sensorName;
		rmDirRec(QDir(path));
		return 0;
	}
	emit storageCreated(id);
	return stor;
}

bool ARpcFSStoragesDatabase::hasStorage(const ARpcStorageId &id)
{
	return storages.contains(id);
}

bool ARpcFSStoragesDatabase::removeStorage(const ARpcStorageId &id)
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
	ARpcISensorStorage *st=storages.take(id);
	emit storageRemoved(id);
	delete st;
	return true;
}

ARpcISensorStorage* ARpcFSStoragesDatabase::findStorageForDevice(
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

QUuid ARpcFSStoragesDatabase::findDeviceId(const QByteArray &devIdOrName)
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
