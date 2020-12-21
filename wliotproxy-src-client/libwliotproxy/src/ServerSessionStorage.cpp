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

#include "ServerSessionStorage.h"

using namespace WLIOT;
using namespace WLIOTClient;

ServerSessionStorage::ServerSessionStorage(
	ServerConnection *conn,AllServerCommands *cmds,const QUuid &devId,const QByteArray &devName,
	const SensorDef &sensor,ISensorStorage::StoreMode storeMode,
	ISensorStorage::TimestampRule tsRule,SensorDef::Type storedType,QObject *parent)
	:ISessionSensorStorage(devId,devName,sensor,storeMode,tsRule,storedType,parent)
{
	commands=cmds;
	srvConn=conn;
	mIsOpened=false;
	mHasValuesCount=false;
}

quint64 ServerSessionStorage::valuesCount()
{
	if(!mIsOpened||mainReadId.isNull())
		return 0;
	if(mHasValuesCount)
		return mHasValuesCount;
	return valuesCount(mainReadId);
}

SensorValue* ServerSessionStorage::valueAt(quint64 index)
{
	if(!mIsOpened||mainReadId.isNull())
		return 0;
	return valueAt(mainReadId,index);
}

bool ServerSessionStorage::listSessions(QList<QUuid> &ids,QByteArrayList &titles)
{
	if(!mIsOpened)
	{
		ids.clear();
		titles.clear();
		return false;
	}
	else return commands->storages()->listSessions(mDeviceId.toByteArray(),mSensor.name,ids,titles);
}

quint64 ServerSessionStorage::valuesCount(const QUuid &sessionId)
{
	if(!mIsOpened)return 0;
	quint64 count=0;
	if(!commands->storages()->getSamplesCount(mDeviceId.toByteArray(),mSensor.name,count,sessionId))
		return 0;
	if(sessionId==mainReadId)
	{
		mHasValuesCount=true;
		mValuesCount=count;
	}
	return count;
}

SensorValue* ServerSessionStorage::valueAt(const QUuid &sessionId,quint64 index)
{
	if(!mIsOpened)return 0;
	VeryBigArray<SensorValue*> vals;
	if(!commands->storages()->getSamples(mDeviceId.toByteArray(),mSensor.name,index,1,1,
		mStoredValuesType,vals,sessionId)||vals.size()==0)
		return 0;
	return vals[0]->mkCopy();
}

bool ServerSessionStorage::setMainReadSessionId(const QUuid &id)
{
	if(!mIsOpened)return false;
	mainReadId=id;
	mHasValuesCount=false;
	return true;
}

bool ServerSessionStorage::setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)
{
	if(!mIsOpened)return false;
	return commands->storages()->setSessionAttr(mDeviceId.toByteArray(),mSensor.name,sessionId,key,val);
}

bool ServerSessionStorage::removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)
{
	if(!mIsOpened)return false;
	return commands->storages()->removeSessionAttr(mDeviceId.toByteArray(),mSensor.name,sessionId,key);
}

bool ServerSessionStorage::getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)
{
	if(!mIsOpened)return false;
	return commands->storages()->getSessionAttr(mDeviceId.toByteArray(),mSensor.name,sessionId,key,val);
}

bool ServerSessionStorage::listSessionAttributes(const QUuid &sessionId,QMap<QByteArray,QByteArray> &map)
{
	if(!mIsOpened)return false;
	return commands->storages()->listSessionAttrs(mDeviceId.toByteArray(),mSensor.name,sessionId,map);
}

bool ServerSessionStorage::removeSession(const QUuid &sessionId)
{
	if(!mIsOpened)return false;
	return commands->storages()->removeSession(mDeviceId.toByteArray(),mSensor.name,sessionId);
}

bool ServerSessionStorage::values(quint64 startIndex,quint64 count,quint64 step,
	VeryBigArray<SensorValue*> &vals)
{
	if(!mIsOpened||mainReadId.isNull())return false;
	return commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,startIndex,count,step,mStoredValuesType,vals,mainReadId);
}

bool ServerSessionStorage::values(const QUuid &sessionId,quint64 startIndex,
	quint64 count,quint64 step,VeryBigArray<SensorValue*> &vals)
{
	if(!mIsOpened)return false;
	return commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,startIndex,count,step,mStoredValuesType,vals,sessionId);
}

void ServerSessionStorage::setClosedWhenSrvDisconnected()
{
	mIsOpened=false;
	mHasValuesCount=false;
	mainReadId=QUuid();
}

SensorValue* ServerSessionStorage::lastValue()
{
	return valueAt(valuesCount()-1);
}

bool ServerSessionStorage::createSession(const QByteArray &title,QUuid &sessionId)
{
	if(!mIsOpened)return false;
	return commands->storages()->createSession(mDeviceId.toByteArray(),mSensor.name,title,sessionId);
}

bool ServerSessionStorage::openSession(const QUuid &sessionId)
{
	Q_UNUSED(sessionId)
	return mIsOpened;
}

bool ServerSessionStorage::closeSession(const QUuid &sessionId)
{
	Q_UNUSED(sessionId)
	return mIsOpened;
}

bool ServerSessionStorage::isSessionOpened(const QUuid &sessionId)const
{
	Q_UNUSED(sessionId)
	return mIsOpened;
}

bool ServerSessionStorage::openMainWriteSession(const QUuid &sessionId)
{
	if(!mIsOpened)return false;
	return commands->storages()->startSession(mDeviceId.toByteArray(),mSensor.name,sessionId);
}

bool ServerSessionStorage::isMainWriteSessionOpened()const
{
	if(!mIsOpened)return false;
	QUuid wrId;
	if(!commands->storages()->getSessionWriteId(mDeviceId.toByteArray(),mSensor.name,wrId))
		return false;
	return !wrId.isNull();
}

bool ServerSessionStorage::closeMainWriteSession()
{
	if(!mIsOpened)return false;
	return commands->storages()->stopSession(mDeviceId.toByteArray(),mSensor.name);
}

QUuid ServerSessionStorage::getMainWriteSessionId()const
{
	if(!mIsOpened)return QUuid();
	QUuid wrId;
	if(!commands->storages()->getSessionWriteId(mDeviceId.toByteArray(),mSensor.name,wrId))
		return QUuid();
	return wrId;
}

bool ServerSessionStorage::writeSensorValue(const SensorValue *val)
{
	Q_UNUSED(val)
	return false;
}

void ServerSessionStorage::writeAttribute(const QByteArray &str,const QByteArray &var)
{
	commands->storages()->setStorageAttr(mDeviceId.toByteArray(),mSensor.name,str,var);
}

QByteArray ServerSessionStorage::readAttribute(const QByteArray &str)
{
	QByteArray v;
	commands->storages()->getStorageAttr(mDeviceId.toByteArray(),mSensor.name,str,v);
	return v;
}

void ServerSessionStorage::addDataExportConfig(const QUuid &serviceId,const DataExportConfig &cfg)
{
	commands->storages()->storageAddDataExport(mDeviceId.toByteArray(),mSensor.name,serviceId,cfg);
}

bool ServerSessionStorage::hasDataExportConfig(const QUuid &serviceId)
{
	return allDataExportServices().contains(serviceId);
}

ISensorStorage::DataExportConfig ServerSessionStorage::getDataExportConfig(const QUuid &serviceId)
{
	ISensorStorage::DataExportConfig cfg;
	commands->storages()->storageGetDataExport(mDeviceId.toByteArray(),mSensor.name,serviceId,cfg);
	return cfg;
}

void ServerSessionStorage::removeDataExportConfig(const QUuid &serviceId)
{
	commands->storages()->storageAddDataExport(
		mDeviceId.toByteArray(),mSensor.name,serviceId,DataExportConfig());
}

QList<QUuid> ServerSessionStorage::allDataExportServices()
{
	QList<QUuid> services;
	commands->storages()->storageAllDataExports(mDeviceId.toByteArray(),mSensor.name,services);
	return services;
}

void ServerSessionStorage::onNewValueFromServer(const QByteArrayList &vArgs)
{
	QScopedPointer<SensorValue> v(SensorValue::createSensorValue(mStoredValuesType));
	if(!v)return;
	if(v->parseMsgArgs(vArgs))
		emit newValueWritten(v.data());
}


bool ServerSessionStorage::open()
{
	if(mIsOpened)return true;
	if(srvConn->subscribeStorage(mDeviceId.toByteArray(),mSensor.name))return false;
	mIsOpened=true;
	return true;
}

void ServerSessionStorage::close()
{
	if(!mIsOpened)return;
	srvConn->unsubscribeStorage(mDeviceId.toByteArray(),mSensor.name);
	mIsOpened=false;
}

bool ServerSessionStorage::isOpened()const
{
	return mIsOpened;
}
