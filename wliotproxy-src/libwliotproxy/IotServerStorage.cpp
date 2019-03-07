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

#include "IotServerStorage.h"

IotServerStorage::IotServerStorage(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,
	const QByteArray &devName,const SensorDef &sensor,ISensorStorage::StoreMode storeMode,
	ISensorStorage::TimestampRule tsRule,SensorDef::Type storedType,QObject *parent)
	:ISensorStorage(devId,devName,sensor,storeMode,tsRule,storedType,parent)
{
	srvConn=conn;
	commands=cmds;
	mIsOpened=false;
}

IotServerStorage::~IotServerStorage()
{
	close();
}

quint64 IotServerStorage::valuesCount()
{
	if(!mIsOpened)return 0;
	quint64 c=0;
	commands->storages()->getSamplesCount(mDeviceId.toByteArray(),mSensor.name,c);
	return c;
}

bool IotServerStorage::isOpened()const
{
	return mIsOpened;
}

bool IotServerStorage::open()
{
	if(mIsOpened)return true;
	if(!srvConn->subscribeStorage(mDeviceId.toByteArray(),mSensor.name))
		return false;
	mIsOpened=true;
	return true;
}

void IotServerStorage::close()
{
	if(!mIsOpened)return;
	srvConn->unsubscribeStorage(mDeviceId.toByteArray(),mSensor.name);
	mIsOpened=false;
}

SensorValue* IotServerStorage::valueAt(quint64 index)
{
	if(!mIsOpened)return 0;
	VeryBigArray<SensorValue*> vals;
	if(!commands->storages()->getSamples(mDeviceId.toByteArray(),mSensor.name,index,1,1,mStoredValuesType,vals))
		return 0;
	if(vals.size()==0)return 0;
	return vals[0]->mkCopy();
}

bool IotServerStorage::writeSensorValue(const SensorValue *val)
{
	Q_UNUSED(val)
	return false;
}

void IotServerStorage::writeAttribute(const QByteArray &str,const QByteArray &var)
{
	commands->storages()->setStorageAttr(mDeviceId.toByteArray(),mSensor.name,str,var);
}

QByteArray IotServerStorage::readAttribute(const QByteArray &str)
{
	QByteArray v;
	commands->storages()->getStorageAttr(mDeviceId.toByteArray(),mSensor.name,str,v);
	return v;
}

void IotServerStorage::addDataExportConfig(
	const QByteArray &serviceType,const ISensorStorage::DataExportConfig &cfg)
{
	commands->storages()->storageAddDataExport(mDeviceId.toByteArray(),mSensor.name,serviceType,cfg);
}

bool IotServerStorage::hasDataExportConfig(const QByteArray &serviceType)
{
	return allDataExportServices().contains(serviceType);
}

ISensorStorage::DataExportConfig IotServerStorage::getDataExportConfig(const QByteArray &serviceType)
{
	ISensorStorage::DataExportConfig cfg;
	commands->storages()->storageGetDataExport(mDeviceId.toByteArray(),mSensor.name,serviceType,cfg);
	return cfg;
}

void IotServerStorage::removeDataExportConfig(const QByteArray &serviceType)
{
	commands->storages()->storageAddDataExport(
		mDeviceId.toByteArray(),mSensor.name,serviceType,DataExportConfig());
}

QByteArrayList IotServerStorage::allDataExportServices()
{
	QByteArrayList services;
	commands->storages()->storageAllDataExports(mDeviceId.toByteArray(),mSensor.name,services);
	return services;
}

bool IotServerStorage::values(quint64 index,quint64 count,quint64 step,
	VeryBigArray<SensorValue*> &vals)
{
	if(!mIsOpened)return false;
	return commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,index,count,step,mStoredValuesType,vals);
}

void IotServerStorage::setClosedWhenSrvDisconnected()
{
	mIsOpened=false;
}

void IotServerStorage::onNewValueFromServer(const QByteArrayList &vArgs)
{
	QScopedPointer<SensorValue> v(SensorValue::createSensorValue(mStoredValuesType));
	if(!v)return;
	if(v->parseMsgArgs(vArgs))
		emit newValueWritten(v.data());
}
