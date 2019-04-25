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

#include "IotServerSessionStorage.h"

IotServerSessionStorage::IotServerSessionStorage(
	IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,const QByteArray &devName,
	const SensorDef &sensor,ISensorStorage::StoreMode storeMode,
	ISensorStorage::TimestampRule tsRule,SensorDef::Type storedType,QObject *parent)
	:ISessionSensorStorage(devId,devName,sensor,storeMode,tsRule,storedType,parent)
{
	commands=cmds;
	srvConn=conn;
}

quint64 IotServerSessionStorage::valuesCount()
{
	if(!mIsOpened||mainReadId.isNull())
		return 0;
	return valuesCount(mainReadId);
}

SensorValue* IotServerSessionStorage::valueAt(quint64 index)
{
	if(!mIsOpened||mainReadId.isNull())
		return 0;
	return valueAt(mainReadId,index);
}

bool IotServerSessionStorage::listSessions(QList<QUuid> &ids,QByteArrayList &titles)
{
	if(!mIsOpened)
	{
		ids.clear();
		titles.clear();
		return false;
	}
	else return commands->storages()->listSessions(mDeviceId.toByteArray(),mSensor.name,ids,titles);
}

quint64 IotServerSessionStorage::valuesCount(const QUuid &sessionId)
{
	if(!mIsOpened)return 0;
	quint64 count=0;
	commands->storages()->getSamplesCount(mDeviceId.toByteArray(),mSensor.name,count,sessionId);
	return count;
}

SensorValue *IotServerSessionStorage::valueAt(const QUuid &sessionId,quint64 index)
{
	if(!mIsOpened)return 0;
	VeryBigArray<SensorValue*> vals;
	if(!commands->storages()->getSamples(mDeviceId.toByteArray(),mSensor.name,index,1,1,
		mStoredValuesType,vals,sessionId)||vals.size()==0)
		return 0;
	return vals[0]->mkCopy();
}

bool IotServerSessionStorage::setMainReadSessionId(const QUuid &id)
{
	if(!mIsOpened)return false;
	mainReadId=id;
	return true;
}

bool IotServerSessionStorage::setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)
{
	if(!mIsOpened)return false;
	return commands->storages()->setSessionAttr(mDeviceId.toByteArray(),mSensor.name,sessionId,key,val);
}

bool IotServerSessionStorage::removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)
{
	if(!mIsOpened)return false;
	return commands->storages()->removeSessionAttr(mDeviceId.toByteArray(),mSensor.name,sessionId,key);
}

bool IotServerSessionStorage::getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)
{
	if(!mIsOpened)return false;
	return commands->storages()->getSessionAttr(mDeviceId.toByteArray(),mSensor.name,sessionId,key,val);
}

bool IotServerSessionStorage::listSessionAttributes(const QUuid &sessionId,QMap<QByteArray,QByteArray> &map)
{
	if(!mIsOpened)return false;
	return commands->storages()->listSessionAttrs(mDeviceId.toByteArray(),mSensor.name,sessionId,map);
}

bool IotServerSessionStorage::removeSession(const QUuid &sessionId)
{
	if(!mIsOpened)return false;
	return commands->storages()->removeSession(mDeviceId.toByteArray(),mSensor.name,sessionId);
}

bool IotServerSessionStorage::values(quint64 startIndex,quint64 count,quint64 step,
	VeryBigArray<SensorValue*> &vals)
{
	if(!mIsOpened||mainReadId.isNull())return false;
	return commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,startIndex,count,step,mStoredValuesType,vals,mainReadId);
}

bool IotServerSessionStorage::values(const QUuid &sessionId,quint64 startIndex,
	quint64 count,quint64 step,VeryBigArray<SensorValue*> &vals)
{
	if(!mIsOpened)return false;
	return commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,startIndex,count,step,mStoredValuesType,vals,sessionId);
}

void IotServerSessionStorage::setClosedWhenSrvDisconnected()
{
	mIsOpened=false;
}

bool IotServerSessionStorage::createSession(const QByteArray &title,QUuid &sessionId)
{
	if(!mIsOpened)return false;
	return commands->storages()->createSession(mDeviceId.toByteArray(),mSensor.name,title,sessionId);
}

bool IotServerSessionStorage::openSession(const QUuid &sessionId)
{
	Q_UNUSED(sessionId)
	return mIsOpened;
}

bool IotServerSessionStorage::closeSession(const QUuid &sessionId)
{
	Q_UNUSED(sessionId)
	return mIsOpened;
}

bool IotServerSessionStorage::isSessionOpened(const QUuid &sessionId)const
{
	Q_UNUSED(sessionId)
	return mIsOpened;
}

bool IotServerSessionStorage::openMainWriteSession(const QUuid &sessionId)
{
	if(!mIsOpened)return false;
	return commands->storages()->startSession(mDeviceId.toByteArray(),mSensor.name,sessionId);
}

bool IotServerSessionStorage::isMainWriteSessionOpened()const
{
	if(!mIsOpened)return false;
	QUuid wrId;
	if(!commands->storages()->getSessionWriteId(mDeviceId.toByteArray(),mSensor.name,wrId))
		return false;
	return !wrId.isNull();
}

bool IotServerSessionStorage::closeMainWriteSession()
{
	if(!mIsOpened)return false;
	return commands->storages()->stopSession(mDeviceId.toByteArray(),mSensor.name);
}

QUuid IotServerSessionStorage::getMainWriteSessionId()const
{
	if(!mIsOpened)return QUuid();
	QUuid wrId;
	if(!commands->storages()->getSessionWriteId(mDeviceId.toByteArray(),mSensor.name,wrId))
		return QUuid();
	return wrId;
}

bool IotServerSessionStorage::writeSensorValue(const SensorValue *val)
{
	Q_UNUSED(val)
	return false;
}

void IotServerSessionStorage::writeAttribute(const QByteArray &str,const QByteArray &var)
{
	commands->storages()->setStorageAttr(mDeviceId.toByteArray(),mSensor.name,str,var);
}

QByteArray IotServerSessionStorage::readAttribute(const QByteArray &str)
{
	QByteArray v;
	commands->storages()->getStorageAttr(mDeviceId.toByteArray(),mSensor.name,str,v);
	return v;
}

void IotServerSessionStorage::addDataExportConfig(const QUuid &serviceId,const DataExportConfig &cfg)
{
	commands->storages()->storageAddDataExport(mDeviceId.toByteArray(),mSensor.name,serviceId,cfg);
}

bool IotServerSessionStorage::hasDataExportConfig(const QUuid &serviceId)
{
	return allDataExportServices().contains(serviceId);
}

ISensorStorage::DataExportConfig IotServerSessionStorage::getDataExportConfig(const QUuid &serviceId)
{
	ISensorStorage::DataExportConfig cfg;
	commands->storages()->storageGetDataExport(mDeviceId.toByteArray(),mSensor.name,serviceId,cfg);
	return cfg;
}

void IotServerSessionStorage::removeDataExportConfig(const QUuid &serviceId)
{
	commands->storages()->storageAddDataExport(
		mDeviceId.toByteArray(),mSensor.name,serviceId,DataExportConfig());
}

QList<QUuid> IotServerSessionStorage::allDataExportServices()
{
	QList<QUuid> services;
	commands->storages()->storageAllDataExports(mDeviceId.toByteArray(),mSensor.name,services);
	return services;
}

void IotServerSessionStorage::onNewValueFromServer(const QByteArrayList &vArgs)
{
	QScopedPointer<SensorValue> v(SensorValue::createSensorValue(mStoredValuesType));
	if(!v)return;
	if(v->parseMsgArgs(vArgs))
		emit newValueWritten(v.data());
}


bool IotServerSessionStorage::open()
{
	if(mIsOpened)return true;
	if(srvConn->subscribeStorage(mDeviceId.toByteArray(),mSensor.name))return false;
	mIsOpened=true;
	return true;
}

void IotServerSessionStorage::close()
{
	if(!mIsOpened)return;
	srvConn->unsubscribeStorage(mDeviceId.toByteArray(),mSensor.name);
	mIsOpened=false;
}

bool IotServerSessionStorage::isOpened()const
{
	return mIsOpened;
}
