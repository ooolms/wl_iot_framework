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

#include "wliot/client/StoragesDatabase.h"
#include "ServerStorage.h"
#include "ServerSessionStorage.h"

using namespace WLIOT;
using namespace WLIOTClient;

StoragesDatabase::StoragesDatabase(ServerConnection *conn,AllServerCommands *cmds)
	:IStoragesDatabase(cmds)
{
	srvConn=conn;
	commands=cmds;

	connect(srvConn,&ServerConnection::preconnected,this,&StoragesDatabase::onServerConnected);
	connect(srvConn,&ServerConnection::disconnected,this,&StoragesDatabase::onServerDisconnected);
	connect(srvConn,&ServerConnection::storageCreated,this,&StoragesDatabase::onStorageCreatedFromServer);
	connect(srvConn,&ServerConnection::storageRemoved,this,&StoragesDatabase::onStorageRemovedFromServer);
	connect(srvConn,&ServerConnection::newSensorValue,this,&StoragesDatabase::onNewValue);
}

bool StoragesDatabase::listStorages(QList<StorageId> &list)
{
	list=storages.keys();
	return true;
}

bool StoragesDatabase::listStoragesWithDevNames(QList<StorageId> &list,QByteArrayList &titles)
{
	list.clear();
	titles.clear();
	for(auto i=storages.begin();i!=storages.end();++i)
	{
		list.append(i.key());
		titles.append(i.value()->deviceName());
	}
	return true;
}

ISensorStorage* StoragesDatabase::existingStorage(const StorageId &id)
{
	return storages.value(id);
}

ISensorStorage* StoragesDatabase::create(const QUuid &devId,const QByteArray &devName,
	ISensorStorage::StoreMode mode,const SensorDef &sensor,ISensorStorage::TimestampRule rule,
	int valuesCount,bool gtIndex)
{
	Q_UNUSED(devName)
	Q_UNUSED(gtIndex)
	if(!commands->storages()->addStorage(devId.toByteArray(),sensor.name,mode,rule,valuesCount))
		return 0;
	return storages.value({devId,sensor.name});
}

bool StoragesDatabase::hasStorage(const StorageId &id)
{
	return storages.contains(id);
}

bool StoragesDatabase::removeStorage(const StorageId &id)
{
	return commands->storages()->removeStorage(id.deviceId.toByteArray(),id.sensorName);
}

void StoragesDatabase::onStorageCreatedFromServer(const StorageDescr &s)
{
	StorageId id={s.deviceId,s.sensor.name};
	if(storages.contains(id))return;
	storages[id]=createWrap(s);
	emit storageCreated(id);
}

void StoragesDatabase::onStorageRemovedFromServer(const StorageId &id)
{
	if(!storages.contains(id))return;
	auto st=storages.take(id);
	emit storageRemoved(id);
	st->deleteLater();
}

void StoragesDatabase::onServerConnected()
{
	QList<StorageDescr> sList;
	commands->storages()->listStorages(sList);
	QSet<WLIOT::StorageId> ids;
	for(auto &s:sList)
	{
		if(s.deviceId.isNull()||s.sensor.name.isEmpty())continue;
		StorageId id={s.deviceId,s.sensor.name};
		ids.insert(id);
		if(!storages.contains(id))
		{
			storages[id]=createWrap(s);
			emit storageCreated(id);
		}
	}
	for(auto i=storages.begin();i!=storages.end();)
	{
		if(!ids.contains(i.key()))
		{
			emit storageRemoved(i.key());
			storages.erase(i);
		}
		else ++i;
	}
}

void StoragesDatabase::onServerDisconnected()
{
	for(ISensorStorage *s:storages)
	{
		if(s->storeMode()==ISensorStorage::AUTO_SESSIONS||s->storeMode()==ISensorStorage::MANUAL_SESSIONS)
			((ServerSessionStorage*)s)->setClosedWhenSrvDisconnected();
		else ((ServerStorage*)s)->setClosedWhenSrvDisconnected();
	}
}

void StoragesDatabase::onNewValue(const StorageId &id,const QByteArrayList &args)
{
	ISensorStorage *st=storages.value(id);
	if(!st)return;
	if(st->storeMode()==ISensorStorage::AUTO_SESSIONS||st->storeMode()==ISensorStorage::MANUAL_SESSIONS)
		((ServerSessionStorage*)st)->onNewValueFromServer(args);
	else ((ServerStorage*)st)->onNewValueFromServer(args);
}

ISensorStorage* StoragesDatabase::createWrap(const StorageDescr &s)
{
	if(s.mode==ISensorStorage::AUTO_SESSIONS||s.mode==ISensorStorage::MANUAL_SESSIONS)
		return new ServerSessionStorage(
			srvConn,commands,s.deviceId,s.deviceName,s.sensor,s.mode,s.tsRule,s.storedValuesType,this);
	else return new ServerStorage(
		srvConn,commands,s.deviceId,s.deviceName,s.sensor,s.mode,s.tsRule,s.storedValuesType,this);
}

QUuid StoragesDatabase::findDeviceId(const QByteArray &devIdOrName)
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

ISensorStorage *StoragesDatabase::findStorageForDevice(
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

QByteArray StoragesDatabase::findDevName(const QUuid &devId)
{
	for(auto i=storages.begin();i!=storages.end();++i)
		if(i.key().deviceId==devId)
			return i.value()->deviceName();
	return QByteArray();
}
