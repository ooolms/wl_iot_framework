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

#include "JSStoragesDatabase.h"
#include "JSSessionsStorage.h"
#include <QScriptValue>

using namespace WLIOT;
using namespace WLIOTClient;

JSStoragesDatabase::JSStoragesDatabase(QScriptEngine *e,ServerInstance *s)
	:QObject(e)
{
	srv=s;
	js=e;
	connect(s->storages(),&StoragesDatabase::storageCreated,this,&JSStoragesDatabase::onStorageCreated);
	connect(s->storages(),&StoragesDatabase::storageRemoved,this,&JSStoragesDatabase::onStorageRemoved);
}

QScriptValue JSStoragesDatabase::listSensors()
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

QScriptValue JSStoragesDatabase::existingStorage(QScriptValue obj)
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

QScriptValue JSStoragesDatabase::createStorage(QScriptValue obj)
{
	if(!obj.isObject())
		return js->nullValue();
	QUuid deviceId=QUuid(obj.property("deviceId").toString());
	QByteArray sensorName=obj.property("sensorName").toString().toUtf8();
	if(deviceId.isNull()||sensorName.isEmpty())
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
	SensorDef sensor;
	sensor.name=sensorName;
	ISensorStorage *stor=srv->storages()->create(deviceId,"",mode,sensor,tsRule,valuesCount);
	if(!stor)
		return js->nullValue();
	return existingStorage(obj);
}

void JSStoragesDatabase::onStorageCreated(const StorageId &id)
{
	ISensorStorage *st=srv->storages()->existingStorage(id);
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

void JSStoragesDatabase::onStorageRemoved(const StorageId &id)
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

JSISensorStorage* JSStoragesDatabase::makeJSStorage(ISensorStorage *st)
{
	if(st->storeMode()==ISensorStorage::AUTO_SESSIONS||
		st->storeMode()==ISensorStorage::MANUAL_SESSIONS)
		return new JSSessionsStorage(js,st,this);
	else return new JSISensorStorage(js,st,this);
}
