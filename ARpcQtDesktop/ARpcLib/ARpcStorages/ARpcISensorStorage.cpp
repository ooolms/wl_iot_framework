#include "ARpcISensorStorage.h"

ARpcISensorStorage::ARpcISensorStorage(const QUuid &devId,const QByteArray &devName,ARpcSensorDef sensor,
	StoreMode stMode,TimestampRule tsRule,ARpcSensorDef::Type storedValType,QObject *parent)
	:QObject(parent)
{
	mDeviceId=devId;
	mDeviceName=devName;
	mSensor=sensor;
	mStoredValuesType=storedValType;
	mTimestampRule=tsRule;
	mStoreMode=stMode;
}

const ARpcSensorDef& ARpcISensorStorage::sensor()const
{
	return mSensor;
}

QUuid ARpcISensorStorage::deviceId()
{
	return mDeviceId;
}

QByteArray ARpcISensorStorage::deviceName()
{
	return mDeviceName;
}

QByteArray ARpcISensorStorage::storeModeToString(ARpcISensorStorage::StoreMode mode)
{
	if(mode==CONTINUOUS)
		return "continuous";
	else if(mode==MANUAL_SESSIONS)
		return "manual_sessions";
	else if(mode==AUTO_SESSIONS)
		return "auto_sessions";
	else if(mode==LAST_N_VALUES)
		return "last_n_values";
	else if(mode==LAST_N_VALUES_IN_MEMORY)
		return "memory";
	else
		return QByteArray();
}

ARpcISensorStorage::StoreMode ARpcISensorStorage::storeModeFromString(const QByteArray &str)
{
	if(str=="continuous")
		return CONTINUOUS;
	else if(str=="manual_sessions")
		return MANUAL_SESSIONS;
	else if(str=="auto_sessions")
		return AUTO_SESSIONS;
	else if(str=="last_n_values")
		return LAST_N_VALUES;
	else if(str=="memory")
		return LAST_N_VALUES_IN_MEMORY;
	else
		return BAD_MODE;
}

QByteArray ARpcISensorStorage::timestampRuleToString(ARpcISensorStorage::TimestampRule rule)
{
	if(rule==ADD_GT)
		return "add_global_time";
	else if(rule==DROP_TIME)
		return "drop_time";
	else /*DONT_TOUCH*/
		return "dont_touch";
}

bool ARpcISensorStorage::timestampRuleFromString(
	const QByteArray &str,ARpcISensorStorage::TimestampRule &rule)
{
	if(str=="add_global_time")
		rule=ADD_GT;
	else if(str=="drop_time")
		rule=DROP_TIME;
	else if(str=="dont_touch")
		rule=DONT_TOUCH;
	else
		return false;
	return true;
}

ARpcISensorStorage::TimestampRule ARpcISensorStorage::timestampRule()const
{
	return mTimestampRule;
}

ARpcSensorDef::Type ARpcISensorStorage::storedValuesType()const
{
	return mStoredValuesType;
}

ARpcISensorStorage::StoreMode ARpcISensorStorage::storeMode()const
{
	return mStoreMode;
}

bool ARpcISensorStorage::values(quint64 index,quint64 count,quint64 step,VeryBigArray<ARpcSensorValue*> &vals)
{
	if(!isOpened())
		return false;
	vals.clear();
	for(quint64 i=0;i<count;++i)
	{
		ARpcSensorValue *v=valueAt(index+i*step);
		if(v)vals.append(v);
		else break;
	}
	return true;
}

ARpcSensorDef::Type ARpcISensorStorage::defaultStoredValuesType(
	ARpcSensorDef::Type sType,ARpcISensorStorage::TimestampRule rule)
{
	ARpcSensorDef::Type retVal=sType;
	if(rule==ADD_GT)
		retVal.tsType=ARpcSensorDef::GLOBAL_TIME;
	else if(rule==DROP_TIME)
		retVal.tsType=ARpcSensorDef::NO_TIME;
	return retVal;
}
