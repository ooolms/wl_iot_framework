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

#include "JSLocalDatabase.h"
#include "JSContinuousStorage.h"
#include "JSSessionsStorage.h"
#include "../ServerInstance.h"
#include "../MainServerConfig.h"
#include <QScriptValue>

using namespace WLIOT;

JSLocalDatabase::JSLocalDatabase(QScriptEngine *e,FSStoragesDatabase *db,IdType uid)
	:QObject(e)
{
	dBase=db;
	js=e;
	ownerUid=uid;
	connect(db,&FSStoragesDatabase::opened,this,&JSLocalDatabase::onOpened);
	connect(db,&FSStoragesDatabase::closed,this,&JSLocalDatabase::onClosed);
	connect(db,&FSStoragesDatabase::storageCreated,this,&JSLocalDatabase::onStorageCreated);
	connect(db,&FSStoragesDatabase::storageRemoved,this,&JSLocalDatabase::onStorageRemoved);
	if(db->isOpened())
		onOpened();
}

bool JSLocalDatabase::isOpened()
{
	return dBase->isOpened();
}

QScriptValue JSLocalDatabase::listSensors()
{
	QList<StorageId> ids=storages.keys();
	QScriptValue retVal=js->newArray(ids.count());
	for(int i=0;i<ids.count();++i)
	{
		QScriptValue val=js->newObject();
		val.setProperty("deviceId",ids[i].deviceId.toString(),QScriptValue::ReadOnly);
		val.setProperty("sensorName",QString::fromUtf8(ids[i].sensorName),QScriptValue::ReadOnly);
		retVal.setProperty(i,val,QScriptValue::ReadOnly);
	}
	return retVal;
}

QScriptValue JSLocalDatabase::existingStorage(QScriptValue obj)
{
	if(!obj.isObject())
		return js->nullValue();
	QUuid deviceId=QUuid(obj.property("deviceId").toString());
	QByteArray sensorName=obj.property("sensorName").toString().toUtf8();
	if(deviceId.isNull()||sensorName.isEmpty())
		return js->nullValue();
	StorageId id={deviceId,sensorName};
	if(!storages.contains(id))
		return js->nullValue();
	return js->newQObject(storages[id]);
}

QScriptValue JSLocalDatabase::createStorage(QScriptValue obj)
{
	if(!obj.isObject())
		return js->nullValue();
	QUuid deviceId=QUuid(obj.property("deviceId").toString());
	QByteArray sensorName=obj.property("sensorName").toString().toUtf8();
	if(deviceId.isNull()||sensorName.isEmpty())
		return js->nullValue();
	if(!MainServerConfig::accessManager.userCanAccessDevice(
		deviceId,ownerUid,DevicePolicyActionFlag::SETUP_STORAGES))
			return js->nullValue();
	ISensorStorage::StoreMode mode=ISensorStorage::storeModeFromString(
		obj.property("storeMode").toString().toUtf8());
	if(mode==ISensorStorage::INVALID_MODE||mode==ISensorStorage::AUTO_SESSIONS)
		return js->nullValue();
	ISensorStorage::TimestampRule tsRule=ISensorStorage::ADD_GT;
	if(obj.property("tsRule").isString())
		if((tsRule=ISensorStorage::timestampRuleFromString(obj.property("tsRule").toString().toUtf8()))==
			ISensorStorage::INVALID_RULE)
			return js->nullValue();
	int valuesCount=1;
	if(obj.property("N").isNumber())
		valuesCount=obj.property("N").toUInt32();
	if(valuesCount<=0)
		valuesCount=1;
	RealDevice *dev=ServerInstance::inst().devices()->deviceById(deviceId);
	if(!dev)
		return js->nullValue();
	QByteArray deviceName=dev->name();
	QList<SensorDef> sensors;
	if(!dev->getSensorsDescription(sensors))
		return js->nullValue();
	SensorDef sensor;
	bool sensorFound=false;
	for(SensorDef &s:sensors)
	{
		if(s.name==sensorName)
		{
			sensor=s;
			sensorFound=true;
			break;
		}
	}
	if(!sensorFound)
		return js->nullValue();
	ISensorStorage *stor=dBase->create(deviceId,deviceName,mode,sensor,tsRule,valuesCount);
	if(!stor)
		return js->nullValue();
	return existingStorage(obj);
}

void JSLocalDatabase::onOpened()
{
	QList<StorageId> ids;
	dBase->listStorages(ids);
	for(int i=0;i<ids.count();++i)
	{
		if(!MainServerConfig::accessManager.userCanAccessDevice(
			ids[i].deviceId,ownerUid,DevicePolicyActionFlag::READ_STORAGES))
				continue;
		ISensorStorage *st=dBase->existingStorage(ids[i]);
		JSISensorStorage *jSt=new JSISensorStorage(js,st,this);
		storages[ids[i]]=jSt;
	}
	emit opened();
}

void JSLocalDatabase::onClosed()
{
	emit closed();
	for(auto i:storages)
		delete i;
	storages.clear();
}

void JSLocalDatabase::onStorageCreated(const StorageId &id)
{
	if(!MainServerConfig::accessManager.userCanAccessDevice(
		id.deviceId,ownerUid,DevicePolicyActionFlag::READ_STORAGES))
			return;
	ISensorStorage *st=dBase->existingStorage(id);
	if(!st)
		return;
	JSISensorStorage *jSt=makeJSStorage(st);
	if(!jSt)
		return;
	storages[id]=jSt;
	QScriptValue val=js->newObject();
	val.setProperty("deviceId",id.deviceId.toString(),QScriptValue::ReadOnly);
	val.setProperty("sensorName",QString::fromUtf8(id.sensorName),QScriptValue::ReadOnly);
	emit storageCreated(val);
}

void JSLocalDatabase::onStorageRemoved(const StorageId &id)
{
	if(!storages.contains(id))
		return;
	QScriptValue val=js->newObject();
	val.setProperty("deviceId",id.deviceId.toString(),QScriptValue::ReadOnly);
	val.setProperty("sensorName",QString::fromUtf8(id.sensorName),QScriptValue::ReadOnly);
	emit storageRemoved(val);
	delete storages[id];
	storages.remove(id);
}

JSISensorStorage* JSLocalDatabase::makeJSStorage(ISensorStorage *st)
{
	if(st->storeMode()==ISensorStorage::CONTINUOUS)
		return new JSContinuousStorage(js,st,this);
	else if(st->storeMode()==ISensorStorage::AUTO_SESSIONS||
		st->storeMode()==ISensorStorage::MANUAL_SESSIONS)
		return new JSSessionsStorage(js,st,this);
	else if(st->storeMode()==ISensorStorage::LAST_N_VALUES_IN_MEMORY)
		return new JSISensorStorage(js,st,this);
	else if(st->storeMode()==ISensorStorage::LAST_N_VALUES)
		return new JSISensorStorage(js,st,this);
	return 0;
}
