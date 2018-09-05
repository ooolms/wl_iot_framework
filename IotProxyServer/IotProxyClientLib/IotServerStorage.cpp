#include "IotServerStorage.h"

IotServerStorage::IotServerStorage(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &devId,
	const QByteArray &devName,const ARpcSensorDef &sensor,ARpcISensorStorage::StoreMode storeMode,
	ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedType,QObject *parent)
	:ARpcISensorStorage(devId,devName,sensor,storeMode,tsRule,storedType,parent)
{
	srvConn=conn;
	commands=cmds;
	mIsOpened=false;
}

IotServerStorage::~IotServerStorage()
{
	close();
}

bool IotServerStorage::isOpened()const
{
	return mIsOpened;
}

bool IotServerStorage::open()
{
	if(mIsOpened)return true;
	if(srvConn->subscribeStorage(mDeviceId.toByteArray(),mSensor.name))return false;
	mIsOpened=true;
	return true;
}

void IotServerStorage::close()
{
	if(!mIsOpened)return;
	srvConn->unsubscribeStorage(mDeviceId.toByteArray(),mSensor.name);
	mIsOpened=false;
}

ARpcSensorValue* IotServerStorage::valueAt(quint64 index)
{
	if(!mIsOpened)return 0;
	VeryBigArray<ARpcSensorValue*> vals;
	if(!commands->storages()->getSamples(mDeviceId.toByteArray(),mSensor.name,index,1,1,mStoredValuesType,vals))
		return 0;
	if(vals.size()==0)return 0;
	return vals[0];
}

bool IotServerStorage::writeSensorValue(const ARpcSensorValue *val)
{
	Q_UNUSED(val)
	return false;
}

void IotServerStorage::writeAttribute(const QByteArray &str,const QVariant &var)
{
	//IMPL
}

QVariant IotServerStorage::readAttribute(const QByteArray &str)
{
	//IMPL
	return QVariant();
}

void IotServerStorage::addDataExportConfig(const QByteArray &serviceType, const ARpcISensorStorage::DataExportConfig &cfg)
{
	//IMPL
}

bool IotServerStorage::hasDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
	return false;
}

ARpcISensorStorage::DataExportConfig IotServerStorage::getDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
	return DataExportConfig();
}

void IotServerStorage::removeDataExportConfig(const QByteArray &serviceType)
{
	//IMPL
}

QByteArrayList IotServerStorage::allDataExportServices()
{
	//IMPL
	return QByteArrayList();
}

bool IotServerStorage::values(quint64 index,quint64 count,quint64 step,VeryBigArray<ARpcSensorValue*> &vals)
{
	if(!mIsOpened)return false;
	return commands->storages()->getSamples(
		mDeviceId.toByteArray(),mSensor.name,index,count,step,mStoredValuesType,vals);
}

void IotServerStorage::setClosedWhenSrvDisconnected()
{
	mIsOpened=false;
}
