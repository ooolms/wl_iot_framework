#include "IotServerStorages.h"
#include "IotServerStorage.h"
#include "IotServerSessionStorage.h"

RemoteStorages::RemoteStorages(IotServerConnection *conn,IotServerCommands *cmds,QObject *parent)
	:QObject(parent)
{
	srvConn=conn;
	commands=cmds;

	connect(srvConn,&IotServerConnection::storageCreated,this,&RemoteStorages::onStorageCreatedFromServer);
	connect(srvConn,&IotServerConnection::storageRemoved,this,&RemoteStorages::onStorageRemovedFromServer);
	connect(srvConn,&IotServerConnection::connected,this,&RemoteStorages::onServerConnected);
	connect(srvConn,&IotServerConnection::disconnected,this,&RemoteStorages::onServerDisconnected);
	connect(srvConn,&IotServerConnection::newSensorValue,this,&RemoteStorages::onNewValue);
}

bool RemoteStorages::listStorages(QList<ARpcStorageId> &list)
{
	list=storages.keys();
	return true;
}

bool RemoteStorages::listStoragesWithDevNames(QList<ARpcStorageId> &list,QByteArrayList &titles)
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

IotServerStorage* RemoteStorages::existingStorage(const ARpcStorageId &id)
{
	return storages.value(id);
}

IotServerStorage* RemoteStorages::create(
	const QUuid &devId,const QByteArray &devName,ARpcISensorStorage::StoreMode mode,
	const ARpcSensorDef &sensor,ARpcISensorStorage::TimestampRule rule,int valuesCount)
{
	Q_UNUSED(devName)
	if(!commands->storages()->addStorage(devId.toByteArray(),sensor.name,mode,rule,valuesCount))
		return 0;
	return storages.value({devId,sensor.name});
}

bool RemoteStorages::hasStorage(const ARpcStorageId &id)
{
	return storages.contains(id);
}

bool RemoteStorages::removeStorage(const ARpcStorageId &id)
{
	return commands->storages()->removeStorage(id.deviceId.toByteArray(),id.sensorName);
}

void RemoteStorages::onStorageCreatedFromServer(const IotServerStorageDescr &s)
{
	ARpcStorageId id={s.deviceId,s.sensor.name};
	if(storages.contains(id))return;
	storages[id]=createWrap(s);
	emit storageCreated(id);
}

void RemoteStorages::onStorageRemovedFromServer(const ARpcStorageId &id)
{
	if(!storages.contains(id))return;
	auto st=storages.take(id);
	emit storageRemoved(id);
	delete st;
}

void RemoteStorages::onServerConnected()
{
	QList<IotServerStorageDescr> sList;
	commands->storages()->listStorages(sList);
	QSet<ARpcStorageId> ids;
	for(auto &s:sList)
	{
		if(s.deviceId.isNull()||s.sensor.name.isEmpty())continue;
		ARpcStorageId id={s.deviceId,s.sensor.name};
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

void RemoteStorages::onServerDisconnected()
{
	for(IotServerStorage *s:storages)
		s->setClosedWhenSrvDisconnected();
}

void RemoteStorages::onNewValue(const ARpcStorageId &id,const QByteArrayList &args)
{
	IotServerStorage *st=storages.value(id);
	if(!st)return;
	if(st->storeMode()==ARpcISensorStorage::AUTO_SESSIONS||st->storeMode()==ARpcISensorStorage::MANUAL_SESSIONS)
		((IotServerSessionStorage*)st)->onNewValueFromServer(args);
	else ((IotServerStorage*)st)->onNewValueFromServer(args);
}

IotServerStorage* RemoteStorages::createWrap(const IotServerStorageDescr &s)
{
	if(s.mode==ARpcISensorStorage::AUTO_SESSIONS||s.mode==ARpcISensorStorage::MANUAL_SESSIONS)
		return new IotServerSessionStorage(
			srvConn,commands,s.deviceId,s.deviceName,s.sensor,s.mode,s.tsRule,s.storedValuesType,this);
	else return new IotServerStorage(
		srvConn,commands,s.deviceId,s.deviceName,s.sensor,s.mode,s.tsRule,s.storedValuesType,this);
}
