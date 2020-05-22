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

#include "ServerStorage.h"

using namespace WLIOT;
using namespace WLIOTClient;

ServerStorage::ServerStorage(ServerConnection *conn,AllServerCommands *cmds,const QUuid &devId,
	const QByteArray &devName,const SensorDef &sensor,ISensorStorage::StoreMode storeMode,
	ISensorStorage::TimestampRule tsRule,SensorDef::Type storedType,QObject *parent)
	:ISensorStorage(devId,devName,sensor,storeMode,tsRule,storedType,parent)
{
	srvConn=conn;
	commands=cmds;
	mIsOpened=false;
}

ServerStorage::~ServerStorage()
{
	close();
}

quint64 ServerStorage::valuesCount()
{
	if(!mIsOpened)return 0;
	quint64 c=0;
	commands->storages()->getSamplesCount(mDeviceId.toByteArray(),mSensor.name,c);
	return c;
}

bool ServerStorage::isOpened()const
{
	return mIsOpened;
}

bool ServerStorage::open()
{
	if(mIsOpened)return true;
	if(!srvConn->subscribeStorage(mDeviceId.toByteArray(),mSensor.name))
		return false;
	mIsOpened=true;
	return true;
}

void ServerStorage::close()
{
	if(!mIsOpened)return;
	srvConn->unsubscribeStorage(mDeviceId.toByteArray(),mSensor.name);
	mIsOpened=false;
}

SensorValue* ServerStorage::valueAt(quint64 index)
{
	if(!mIsOpened)return 0;
	VeryBigArray<SensorValue*> vals;
	if(!commands->storages()->getSamples(mDeviceId.toByteArray(),mSensor.name,index,1,1,mStoredValuesType,vals))
		return 0;
	if(vals.size()==0)return 0;
	return vals[0]->mkCopy();
}

bool ServerStorage::writeSensorValue(const SensorValue *val)
{
	Q_UNUSED(val)
	return false;
}

void ServerStorage::writeAttribute(const QByteArray &str,const QByteArray &var)
{
	commands->storages()->setStorageAttr(mDeviceId.toByteArray(),mSensor.name,str,var);
}

QByteArray ServerStorage::readAttribute(const QByteArray &str)
{
	QByteArray v;
	commands->storages()->getStorageAttr(mDeviceId.toByteArray(),mSensor.name,str,v);
	return v;
}

void ServerStorage::addDataExportConfig(const QUuid &serviceId,const ISensorStorage::DataExportConfig &cfg)
{
	commands->storages()->storageAddDataExport(mDeviceId.toByteArray(),mSensor.name,serviceId,cfg);
}

bool ServerStorage::hasDataExportConfig(const QUuid &serviceId)
{
	return allDataExportServices().contains(serviceId);
}

ISensorStorage::DataExportConfig ServerStorage::getDataExportConfig(const QUuid &serviceId)
{
	ISensorStorage::DataExportConfig cfg;
	commands->storages()->storageGetDataExport(mDeviceId.toByteArray(),mSensor.name,serviceId,cfg);
	return cfg;
}

void ServerStorage::removeDataExportConfig(const QUuid &serviceId)
{
	commands->storages()->storageAddDataExport(
		mDeviceId.toByteArray(),mSensor.name,serviceId,DataExportConfig());
}

QList<QUuid> ServerStorage::allDataExportServices()
{
	QList<QUuid> services;
	commands->storages()->storageAllDataExports(mDeviceId.toByteArray(),mSensor.name,services);
	return services;
}

bool ServerStorage::values(quint64 index,quint64 count,quint64 step,
	VeryBigArray<SensorValue*> &vals)
{
	if(!mIsOpened)return false;
	return commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,index,count,step,mStoredValuesType,vals);
}

void ServerStorage::setClosedWhenSrvDisconnected()
{
	mIsOpened=false;
}

void ServerStorage::onNewValueFromServer(const QByteArrayList &vArgs)
{
	QScopedPointer<SensorValue> v(SensorValue::createSensorValue(mStoredValuesType));
	if(!v)return;
	if(v->parseMsgArgs(vArgs))
		emit newValueWritten(v.data());
}
