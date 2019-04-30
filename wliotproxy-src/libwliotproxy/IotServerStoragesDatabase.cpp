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

#include "IotServerStoragesDatabase.h"
#include "IotServerStorage.h"
#include "IotServerSessionStorage.h"

IotServerStoragesDatabase::IotServerStoragesDatabase(IotServerConnection *conn,IotServerCommands *cmds)
	:IStoragesDatabase(cmds)
{
	srvConn=conn;
	commands=cmds;

	connect(srvConn,&IotServerConnection::preconnected,this,&IotServerStoragesDatabase::onServerConnected);
	connect(srvConn,&IotServerConnection::disconnected,this,&IotServerStoragesDatabase::onServerDisconnected);
	connect(srvConn,&IotServerConnection::storageCreated,this,&IotServerStoragesDatabase::onStorageCreatedFromServer);
	connect(srvConn,&IotServerConnection::storageRemoved,this,&IotServerStoragesDatabase::onStorageRemovedFromServer);
	connect(srvConn,&IotServerConnection::newSensorValue,this,&IotServerStoragesDatabase::onNewValue);
}

bool IotServerStoragesDatabase::listStorages(QList<StorageId> &list)
{
	list=storages.keys();
	return true;
}

bool IotServerStoragesDatabase::listStoragesWithDevNames(QList<StorageId> &list,QByteArrayList &titles)
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

ISensorStorage* IotServerStoragesDatabase::existingStorage(const StorageId &id)
{
	return storages.value(id);
}

ISensorStorage* IotServerStoragesDatabase::create(const QUuid &devId,const QByteArray &devName,
	ISensorStorage::StoreMode mode,const SensorDef &sensor,ISensorStorage::TimestampRule rule,
	int valuesCount,bool gtIndex)
{
	Q_UNUSED(devName)
	Q_UNUSED(gtIndex)
	if(!commands->storages()->addStorage(devId.toByteArray(),sensor.name,mode,rule,valuesCount))
		return 0;
	return storages.value({devId,sensor.name});
}

bool IotServerStoragesDatabase::hasStorage(const StorageId &id)
{
	return storages.contains(id);
}

bool IotServerStoragesDatabase::removeStorage(const StorageId &id)
{
	return commands->storages()->removeStorage(id.deviceId.toByteArray(),id.sensorName);
}

void IotServerStoragesDatabase::onStorageCreatedFromServer(const IotServerStorageDescr &s)
{
	StorageId id={s.deviceId,s.sensor.name};
	if(storages.contains(id))return;
	storages[id]=createWrap(s);
	emit storageCreated(id);
}

void IotServerStoragesDatabase::onStorageRemovedFromServer(const StorageId &id)
{
	if(!storages.contains(id))return;
	auto st=storages.take(id);
	emit storageRemoved(id);
	st->deleteLater();
}

void IotServerStoragesDatabase::onServerConnected()
{
	QList<IotServerStorageDescr> sList;
	commands->storages()->listStorages(sList);
	QSet<StorageId> ids;
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

void IotServerStoragesDatabase::onServerDisconnected()
{
	for(ISensorStorage *s:storages)
	{
		if(s->storeMode()==ISensorStorage::AUTO_SESSIONS||s->storeMode()==ISensorStorage::MANUAL_SESSIONS)
			((IotServerSessionStorage*)s)->setClosedWhenSrvDisconnected();
		else ((IotServerStorage*)s)->setClosedWhenSrvDisconnected();
	}
}

void IotServerStoragesDatabase::onNewValue(const StorageId &id,const QByteArrayList &args)
{
	ISensorStorage *st=storages.value(id);
	if(!st)return;
	if(st->storeMode()==ISensorStorage::AUTO_SESSIONS||st->storeMode()==ISensorStorage::MANUAL_SESSIONS)
		((IotServerSessionStorage*)st)->onNewValueFromServer(args);
	else ((IotServerStorage*)st)->onNewValueFromServer(args);
}

ISensorStorage* IotServerStoragesDatabase::createWrap(const IotServerStorageDescr &s)
{
	if(s.mode==ISensorStorage::AUTO_SESSIONS||s.mode==ISensorStorage::MANUAL_SESSIONS)
		return new IotServerSessionStorage(
			srvConn,commands,s.deviceId,s.deviceName,s.sensor,s.mode,s.tsRule,s.storedValuesType,this);
	else return new IotServerStorage(
		srvConn,commands,s.deviceId,s.deviceName,s.sensor,s.mode,s.tsRule,s.storedValuesType,this);
}

QUuid IotServerStoragesDatabase::findDeviceId(const QByteArray &devIdOrName)
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

ISensorStorage *IotServerStoragesDatabase::findStorageForDevice(
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
