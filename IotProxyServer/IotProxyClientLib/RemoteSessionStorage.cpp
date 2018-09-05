#include "RemoteSessionStorage.h"

RemoteSessionStorage::RemoteSessionStorage(
	IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,const QByteArray &devName,
	const ARpcSensorDef &sensor,ARpcISensorStorage::StoreMode storeMode,
	ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedType,QObject *parent)
	:RemoteStorage(conn,cmds,devId,devName,sensor,storeMode,tsRule,storedType,parent)
{
}

quint64 RemoteSessionStorage::valuesCount()
{
	if(!mIsOpened||mainReadId.isNull())
		return 0;
	return valuesCount(mainReadId);
}

ARpcSensorValue* RemoteSessionStorage::valueAt(quint64 index)
{
	if(!mIsOpened||mainReadId.isNull())
		return 0;
	return valueAt(mainReadId,index);
}

bool RemoteSessionStorage::listSessions(QList<QUuid> &ids,QByteArrayList &titles)
{
	if(!mIsOpened)
	{
		ids.clear();
		titles.clear();
		return false;
	}
	else return commands->storages()->listSessions(mDeviceId.toByteArray(),mSensor.name,ids,titles);
}

quint64 RemoteSessionStorage::valuesCount(const QUuid &sessionId)
{
	if(!mIsOpened)return 0;
	quint64 count=0;
	commands->storages()->getSamplesCount(mDeviceId.toByteArray(),mSensor.name,count,sessionId);
	return count;
}

ARpcSensorValue *RemoteSessionStorage::valueAt(const QUuid &sessionId,quint64 index)
{
	if(!mIsOpened)return 0;
	QList<ARpcSensorValue*> vals;
	if(!commands->storages()->getSamples(mDeviceId.toByteArray(),mSensor.name,index,1,1,
		mStoredValuesType,vals,sessionId)||vals.isEmpty())
		return 0;
	return vals[0];
}

bool RemoteSessionStorage::setMainReadSessionId(const QUuid &id)
{
	mainReadId=id;
	return true;
}

bool RemoteSessionStorage::setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)
{
	return commands->storages()->setSessionAttr(mDeviceId.toByteArray(),mSensor.name,sessionId,key,val);
}

bool RemoteSessionStorage::removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)
{
	return commands->storages()->removeSessionAttr(mDeviceId.toByteArray(),mSensor.name,sessionId,key);
}

bool RemoteSessionStorage::getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)
{
	return commands->storages()->getSessionAttr(mDeviceId.toByteArray(),mSensor.name,sessionId,key,val);
}

bool RemoteSessionStorage::listSessionAttributes(const QUuid &sessionId,QMap<QByteArray,QByteArray> &map)
{
	return commands->storages()->listSessionAttrs(mDeviceId.toByteArray(),mSensor.name,sessionId,map);
}

bool RemoteSessionStorage::removeSession(const QUuid &sessionId)
{
	return commands->storages()->removeSession(mDeviceId.toByteArray(),mSensor.name,sessionId);
}

QList<ARpcSensorValue*> RemoteSessionStorage::values(quint64 startIndex,quint64 count,quint64 step)
{
	QList<ARpcSensorValue*> vals;
	if(!mIsOpened||mainReadId.isNull())return vals;
	if(!commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,startIndex,count,step,mStoredValuesType,vals,mainReadId))
		return QList<ARpcSensorValue*>();
	return vals;
}

QList<ARpcSensorValue*> RemoteSessionStorage::values(const QUuid &sessionId,quint64 startIndex,
	quint64 count,quint64 step)
{
	QList<ARpcSensorValue*> vals;
	if(!mIsOpened)return vals;
	if(!commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,startIndex,count,step,mStoredValuesType,vals,sessionId))
		return QList<ARpcSensorValue*>();
	return vals;
}

bool RemoteSessionStorage::createSession(const QByteArray &title,QUuid &sessionId)
{
	if(!mIsOpened)return false;
	return commands->storages()->createSession(mDeviceId.toByteArray(),mSensor.name,title,sessionId);
}

bool RemoteSessionStorage::openSession(const QUuid &sessionId)
{
	Q_UNUSED(sessionId)
	return mIsOpened;
}

bool RemoteSessionStorage::closeSession(const QUuid &sessionId)
{
	Q_UNUSED(sessionId)
	return mIsOpened;
}

bool RemoteSessionStorage::isSessionOpened(const QUuid &sessionId)const
{
	Q_UNUSED(sessionId)
	return mIsOpened;
}

bool RemoteSessionStorage::openMainWriteSession(const QUuid &sessionId)
{
	if(!mIsOpened)return false;
	return commands->storages()->startSession(mDeviceId.toByteArray(),mSensor.name,sessionId);
}

bool RemoteSessionStorage::isMainWriteSessionOpened()const
{
	if(!mIsOpened)return false;
	QUuid wrId;
	if(!commands->storages()->getSessionWriteId(mDeviceId.toByteArray(),mSensor.name,wrId))
		return false;
	return !wrId.isNull();
}

bool RemoteSessionStorage::closeMainWriteSession()
{
	if(!mIsOpened)return false;
	return commands->storages()->stopSession(mDeviceId.toByteArray(),mSensor.name);
}

QUuid RemoteSessionStorage::getMainWriteSessionId()const
{
	if(!mIsOpened)return QUuid();
	QUuid wrId;
	if(!commands->storages()->getSessionWriteId(mDeviceId.toByteArray(),mSensor.name,wrId))
		return QUuid();
	return wrId;
}

bool RemoteSessionStorage::writeSensorValue(const ARpcSensorValue *val)
{
	return false;
}

void RemoteSessionStorage::writeAttribute(const QByteArray &str,const QVariant &var)
{
	//IMPL
}

QVariant RemoteSessionStorage::readAttribute(const QByteArray &str)
{
	//IMPL
	return QVariant();
}

void RemoteSessionStorage::addDataExportConfig(const QByteArray &serviceType, const DataExportConfig &cfg)
{
	//IMPL
}

bool RemoteSessionStorage::hasDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
	return false;
}

ARpcISensorStorage::DataExportConfig RemoteSessionStorage::getDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
	return DataExportConfig();
}

void RemoteSessionStorage::removeDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
}

QByteArrayList RemoteSessionStorage::allDataExportServices()
{
	//IMPL
	return QByteArrayList();
}

void RemoteStorage::onNewValueFromServer(const QByteArrayList &vArgs)
{
	QScopedPointer<ARpcSensorValue> v(ARpcSensorValue::createSensorValue(mStoredValuesType));
	if(!v)return;
	if(v->parseMsgArgs(vArgs))
		emit newValueWritten(v.data());
}
