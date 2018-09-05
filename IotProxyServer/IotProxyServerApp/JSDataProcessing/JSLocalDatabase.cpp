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
#include "../IotProxyInstance.h"
#include <QScriptValue>

JSLocalDatabase::JSLocalDatabase(QScriptEngine *e,ARpcLocalDatabase *db,QObject *parent)
	:QObject(parent)
{
	dBase=db;
	js=e;
	connect(db,&ARpcLocalDatabase::opened,this,&JSLocalDatabase::onOpened);
	connect(db,&ARpcLocalDatabase::closed,this,&JSLocalDatabase::onClosed);
	connect(db,&ARpcLocalDatabase::storageCreated,this,&JSLocalDatabase::onStorageCreated);
	connect(db,&ARpcLocalDatabase::storageRemoved,this,&JSLocalDatabase::onStorageRemoved);
	if(db->isOpened())
		onOpened();
}

bool JSLocalDatabase::isOpened()
{
	return dBase->isOpened();
}

QScriptValue JSLocalDatabase::listSensors()
{
	QList<ARpcStorageId> ids;
	dBase->listSensors(ids);
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
	ARpcStorageId id={deviceId,sensorName};
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
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(
		obj.property("storeMode").toString().toUtf8());
	if(mode==ARpcISensorStorage::BAD_MODE||mode==ARpcISensorStorage::AUTO_SESSIONS)
		return js->nullValue();
	ARpcISensorStorage::TimestampRule tsRule=ARpcISensorStorage::ADD_GT;
	if(obj.property("tsRule").isString())
		if(!ARpcISensorStorage::timestampRuleFromString(obj.property("tsRule").toString().toUtf8(),tsRule))
			return js->nullValue();
	int valuesCount=1;
	if(obj.property("N").isNumber())
		valuesCount=obj.property("N").toUInt32();
	if(valuesCount<=0)
		valuesCount=1;
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceById(deviceId);
	if(!dev)
		return js->nullValue();
	QByteArray deviceName=dev->name();
	QList<ARpcSensorDef> sensors;
	if(!dev->getSensorsDescription(sensors))
		return js->nullValue();
	ARpcSensorDef sensor;
	bool sensorFound=false;
	for(ARpcSensorDef &s:sensors)
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
	ARpcISensorStorage *stor=dBase->create(deviceId,deviceName,mode,sensor,tsRule,valuesCount);
	if(!stor)
		return js->nullValue();
	return existingStorage(obj);
}

void JSLocalDatabase::onOpened()
{
	QList<ARpcStorageId> ids;
	dBase->listSensors(ids);
	for(int i=0;i<ids.count();++i)
	{
		ARpcISensorStorage *st=dBase->existingStorage(ids[i]);
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

void JSLocalDatabase::onStorageCreated(const ARpcStorageId &id)
{
	ARpcISensorStorage *st=dBase->existingStorage(id);
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

void JSLocalDatabase::onStorageRemoved(const ARpcStorageId &id)
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

JSISensorStorage* JSLocalDatabase::makeJSStorage(ARpcISensorStorage *st)
{
	if(st->storeMode()==ARpcISensorStorage::CONTINUOUS)
		return new JSContinuousStorage(js,st,this);
	else if(st->storeMode()==ARpcISensorStorage::AUTO_SESSIONS||
		st->storeMode()==ARpcISensorStorage::MANUAL_SESSIONS)
		return new JSSessionsStorage(js,st,this);
	else if(st->storeMode()==ARpcISensorStorage::LAST_N_VALUES_IN_MEMORY)
		return new JSISensorStorage(js,st,this);
	else if(st->storeMode()==ARpcISensorStorage::LAST_N_VALUES)
		return new JSISensorStorage(js,st,this);
	return 0;
}
