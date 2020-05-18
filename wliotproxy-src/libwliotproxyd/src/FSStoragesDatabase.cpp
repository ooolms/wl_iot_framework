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

#include "wliot/storages/FSStoragesDatabase.h"
#include "wliot/storages/AllStorages.h"
#include "FSSensorStorageHelper.h"

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

FSStoragesDatabase::FSStoragesDatabase(QObject *parent)
	:IStoragesDatabase(parent)
{
	mOpened=false;
}

bool FSStoragesDatabase::open(const QString &path)
{
	close();
	dbDir=QDir(path);
	if(!dbDir.exists())
		return false;
	QStringList dirs=dbDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString &d:dirs)
	{
		QString path=dbDir.absolutePath()+"/"+d;
		ISensorStorage *st=FSSensorStorageHelper::preOpen(path);
		if(!st)
			continue;
		StorageId id={st->deviceId(),st->sensor().name};
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

void FSStoragesDatabase::close()
{
	if(!mOpened)
		return;
	mOpened=false;
	emit closed();
	for(auto v:storages)
		delete v;
	storages.clear();
}

bool FSStoragesDatabase::isOpened()
{
	return mOpened;
}

bool FSStoragesDatabase::listStorages(QList<StorageId> &list)
{
	if(!mOpened)
		return false;
	list=storages.keys();
	return true;
}

bool FSStoragesDatabase::listStoragesWithDevNames(QList<StorageId> &list,QByteArrayList &titles)
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

ISensorStorage* FSStoragesDatabase::existingStorage(const StorageId &id)
{
	if(!mOpened)
		return 0;
	return storages.value(id);
}

ISensorStorage* FSStoragesDatabase::preCreate(const QUuid &devId,const QByteArray &devName,
	ISensorStorage::StoreMode storeMode,const SensorDef &sensor,
	ISensorStorage::TimestampRule rule)
{
	if(!mOpened)
		return 0;
	StorageId id={devId,sensor.name};
	if(devId.isNull()||sensor.name.isEmpty()||storages.contains(id))
		return 0;
	QString path=dbDir.absolutePath()+"/"+devId.toString()+"_"+QString::fromUtf8(sensor.name);
	QFileInfo info(path);
	if(info.exists())
		return 0;
	ISensorStorage *retVal=FSSensorStorageHelper::preCreate(
		path,devId,devName,sensor,storeMode,rule);
	if(retVal)
		storages[id]=retVal;
	return retVal;
}

ISensorStorage* FSStoragesDatabase::create(const QUuid &devId,const QByteArray &devName,
	ISensorStorage::StoreMode mode,const SensorDef &sensor,
	ISensorStorage::TimestampRule rule,int valuesCount,bool gtIndex)
{
	StorageId id={devId,sensor.name};
	if(storages.contains(id))return 0;
	ISensorStorage *stor=preCreate(devId,devName,mode,sensor,rule);
	if(!stor)
		return 0;
	bool ok=false;
	if(mode==ISensorStorage::CONTINUOUS)
		ok=((ContinuousStorage*)stor)->create(gtIndex);
	else if(mode==ISensorStorage::AUTO_SESSIONS||mode==ISensorStorage::MANUAL_SESSIONS)
		ok=((SessionStorage*)stor)->create(gtIndex);
	else if(mode==ISensorStorage::LAST_N_VALUES)
		ok=((LastNValuesStorage*)stor)->create(valuesCount);
	else if(mode==ISensorStorage::LAST_N_VALUES_IN_MEMORY)
		ok=((LastNValuesInMemoryStorage*)stor)->create(valuesCount);
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

bool FSStoragesDatabase::hasStorage(const StorageId &id)
{
	return storages.contains(id);
}

bool FSStoragesDatabase::removeStorage(const StorageId &id)
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
	ISensorStorage *st=storages.take(id);
	emit storageRemoved(id);
	delete st;
	return true;
}

ISensorStorage* FSStoragesDatabase::findStorageForDevice(
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

QUuid FSStoragesDatabase::findDeviceId(const QByteArray &devIdOrName)
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

QByteArray FSStoragesDatabase::findDevName(const QUuid &devId)
{
	for(auto i=storages.begin();i!=storages.end();++i)
	{
		if(i.key().deviceId==devId)
			return i.value()->deviceName();
	}
	return QByteArray();
}
