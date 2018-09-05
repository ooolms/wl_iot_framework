#include "IotServerSessionStorage.h"

IotServerSessionStorage::IotServerSessionStorage(
	IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,const QByteArray &devName,
	const ARpcSensorDef &sensor,ARpcISensorStorage::StoreMode storeMode,
	ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedType,QObject *parent)
	:IotServerStorage(conn,cmds,devId,devName,sensor,storeMode,tsRule,storedType,parent)
{
}

quint64 IotServerSessionStorage::valuesCount()
{
	if(!mIsOpened||mainReadId.isNull())
		return 0;
	return valuesCount(mainReadId);
}

ARpcSensorValue* IotServerSessionStorage::valueAt(quint64 index)
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

ARpcSensorValue *IotServerSessionStorage::valueAt(const QUuid &sessionId,quint64 index)
{
	if(!mIsOpened)return 0;
	VeryBigArray<ARpcSensorValue*> vals;
	if(!commands->storages()->getSamples(mDeviceId.toByteArray(),mSensor.name,index,1,1,
		mStoredValuesType,vals,sessionId)||vals.size()==0)
		return 0;
	return vals[0];
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

bool IotServerSessionStorage::values(quint64 startIndex,quint64 count,quint64 step,VeryBigArray<ARpcSensorValue*> &vals)
{
	if(!mIsOpened||mainReadId.isNull())return false;
	return commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,startIndex,count,step,mStoredValuesType,vals,mainReadId);
}

bool IotServerSessionStorage::values(const QUuid &sessionId,quint64 startIndex,
	quint64 count,quint64 step,VeryBigArray<ARpcSensorValue*> &vals)
{
	if(!mIsOpened)return false;
	return commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,startIndex,count,step,mStoredValuesType,vals,sessionId);
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

bool IotServerSessionStorage::writeSensorValue(const ARpcSensorValue *val)
{
	return false;
}

void IotServerSessionStorage::writeAttribute(const QByteArray &str,const QVariant &var)
{
	//IMPL
}

QVariant IotServerSessionStorage::readAttribute(const QByteArray &str)
{
	//IMPL
	return QVariant();
}

void IotServerSessionStorage::addDataExportConfig(const QByteArray &serviceType, const DataExportConfig &cfg)
{
	//IMPL
}

bool IotServerSessionStorage::hasDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
	return false;
}

ARpcISensorStorage::DataExportConfig IotServerSessionStorage::getDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
	return DataExportConfig();
}

void IotServerSessionStorage::removeDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
}

QByteArrayList IotServerSessionStorage::allDataExportServices()
{
	//IMPL
	return QByteArrayList();
}

void IotServerStorage::onNewValueFromServer(const QByteArrayList &vArgs)
{
	QScopedPointer<ARpcSensorValue> v(ARpcSensorValue::createSensorValue(mStoredValuesType));
	if(!v)return;
	if(v->parseMsgArgs(vArgs))
		emit newValueWritten(v.data());
}
