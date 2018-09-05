#include "RemoteStorage.h"

RemoteStorage::RemoteStorage(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,
	const QByteArray &devName,const ARpcSensorDef &sensor,ARpcISensorStorage::StoreMode storeMode,
	ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedType,QObject *parent)
	:ARpcISensorStorage(devId,devName,sensor,storeMode,tsRule,storedType,parent)
{
	srvConn=conn;
	commands=cmds;
	mIsOpened=false;
}

RemoteStorage::~RemoteStorage()
{
	close();
}

bool RemoteStorage::isOpened()const
{
	return mIsOpened;
}

bool RemoteStorage::open()
{
	if(mIsOpened)return true;
	if(srvConn->subscribeStorage(mDeviceId.toByteArray(),mSensor.name))return false;
	mIsOpened=true;
	return true;
}

void RemoteStorage::close()
{
	if(!mIsOpened)return;
	srvConn->unsubscribeStorage(mDeviceId.toByteArray(),mSensor.name);
	mIsOpened=false;
}

ARpcSensorValue* RemoteStorage::valueAt(quint64 index)
{
	QList<ARpcSensorValue*> vals;
	if(!commands->storages()->getSamples(mDeviceId.toByteArray(),mSensor.name,index,1,1,mStoredValuesType,vals))
		return 0;
	if(vals.isEmpty())return 0;
	return vals[0];
}

bool RemoteStorage::writeSensorValue(const ARpcSensorValue *val)
{
	Q_UNUSED(val)
	return false;
}

void RemoteStorage::writeAttribute(const QByteArray &str,const QVariant &var)
{
	//IMPL
}

QVariant RemoteStorage::readAttribute(const QByteArray &str)
{
	//IMPL
	return QVariant();
}

void RemoteStorage::addDataExportConfig(const QByteArray &serviceType, const ARpcISensorStorage::DataExportConfig &cfg)
{
	//IMPL
}

bool RemoteStorage::hasDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
	return false;
}

ARpcISensorStorage::DataExportConfig RemoteStorage::getDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
	return DataExportConfig();
}

void RemoteStorage::removeDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
}

QByteArrayList RemoteStorage::allDataExportServices()
{
	//IMPL
	return QByteArrayList();
}

QList<ARpcSensorValue*> RemoteStorage::values(quint64 index,quint64 count,quint64 step)
{
	QList<ARpcSensorValue*> vals;
	commands->storages()->getSamples(mDeviceId.toByteArray(),mSensor.name,index,count,step,mStoredValuesType,vals);
	return vals;
}

void RemoteStorage::setClosedWhenSrvDisconnected()
{
	mIsOpened=false;
}
