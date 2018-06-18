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
#include "ARpcBase/ARpcAllSensorValues.h"
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
	QList<DeviceStorageId> ids;
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
	DeviceStorageId id={deviceId,sensorName};
	int index=storagesIds.indexOf(id);
	if(index==-1)
		return js->nullValue();
	return js->newQObject(storages[index]);
}

QScriptValue JSLocalDatabase::createStorage(QScriptValue obj)
{
	if(!obj.isObject())
		return js->nullValue();
	QUuid deviceId=QUuid(obj.property("deviceId").toString());
	QByteArray deviceName=obj.property("deviceName").toString().toUtf8();
	QByteArray sensorName=obj.property("sensorName").toString().toUtf8();
	if(deviceId.isNull()||sensorName.isEmpty()||deviceName.isEmpty())
		return js->nullValue();
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(
		obj.property("storeMode").toString().toUtf8());
	if(mode!=ARpcISensorStorage::CONTINUOUS&&mode!=ARpcISensorStorage::MANUAL_SESSIONS&&
		mode!=ARpcISensorStorage::LAST_N_VALUES&&mode!=ARpcISensorStorage::LAST_VALUE_IN_MEMORY)
		return js->nullValue();
	ARpcISensorStorage::TimestampRule tsRule=ARpcISensorStorage::ADD_GT;
	if(obj.property("tsRule").isString())
		if(!ARpcISensorStorage::timestampRuleFromString(obj.property("tsRule").toString().toUtf8(),tsRule))
			return js->nullValue();
	int nForLastNValues=1;
	if(obj.property("N").isNumber())
		nForLastNValues=obj.property("N").toUInt32();
	if(nForLastNValues<=0)
		nForLastNValues=1;
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceById(deviceId);
	if(!dev)
		return js->nullValue();
	QList<ARpcSensor> sensors;
	if(!dev->getSensorsDescription(sensors))
		return js->nullValue();
	ARpcSensor sensor;
	bool sensorFound=false;
	for(ARpcSensor &s:sensors)
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
	quint32 dims=1;
	if(sensor.constraints.contains("dims"))
		dims=sensor.constraints["dims"].toUInt();
	if(dims==0)
		dims=1;
	ARpcISensorStorage *stor=dBase->create(deviceId,deviceName,mode,sensor,tsRule,nForLastNValues);
	if(!stor)
		return js->nullValue();
	return existingStorage(obj);
}

void JSLocalDatabase::onOpened()
{
	dBase->listSensors(storagesIds);
	for(int i=0;i<storagesIds.count();++i)
	{
		ARpcISensorStorage *st=dBase->existingStorage(storagesIds[i]);
		JSISensorStorage *jSt=new JSISensorStorage(js,st,this);
		storages.append(jSt);
	}
	emit opened();
}

void JSLocalDatabase::onClosed()
{
	emit closed();
	for(int i=0;i<storages.count();++i)
		delete storages[i];
	storagesIds.clear();
	storages.clear();
}

void JSLocalDatabase::onStorageCreated(const DeviceStorageId &id)
{
	ARpcISensorStorage *st=dBase->existingStorage(id);
	if(!st)
		return;
	JSISensorStorage *jSt=makeJSStorage(st);
	if(!jSt)
		return;
	storagesIds.append(id);
	storages.append(jSt);
	QScriptValue val=js->newObject();
	val.setProperty("deviceId",id.deviceId.toString(),QScriptValue::ReadOnly);
	val.setProperty("sensorName",QString::fromUtf8(id.sensorName),QScriptValue::ReadOnly);
	emit storageCreated(val);
}

void JSLocalDatabase::onStorageRemoved(const DeviceStorageId &id)
{
	int index=storagesIds.indexOf(id);
	if(index==-1)
		return;
	QScriptValue val=js->newObject();
	val.setProperty("deviceId",id.deviceId.toString(),QScriptValue::ReadOnly);
	val.setProperty("sensorName",QString::fromUtf8(id.sensorName),QScriptValue::ReadOnly);
	emit storageRemoved(val);
	storagesIds.removeAt(index);
	delete storages[index];
	storages.removeAt(index);
}

JSISensorStorage* JSLocalDatabase::makeJSStorage(ARpcISensorStorage *st)
{
	if(st->getStoreMode()==ARpcISensorStorage::CONTINUOUS)
		return new JSContinuousStorage(js,st,this);
	else if(st->getStoreMode()==ARpcISensorStorage::AUTO_SESSIONS||
		st->getStoreMode()==ARpcISensorStorage::MANUAL_SESSIONS)
		return new JSSessionsStorage(js,st,this);
	else if(st->getStoreMode()==ARpcISensorStorage::LAST_VALUE_IN_MEMORY)
		return new JSISensorStorage(js,st,this);
	else if(st->getStoreMode()==ARpcISensorStorage::LAST_N_VALUES)
		return new JSISensorStorage(js,st,this);
	return 0;
}
