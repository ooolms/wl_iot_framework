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

#include "wliot/storages/ISensorStorage.h"

ISensorStorage::ISensorStorage(const QUuid &devId,const QByteArray &devName,SensorDef sensor,
	StoreMode stMode,TimestampRule tsRule,SensorDef::Type storedValType,QObject *parent)
	:QObject(parent)
{
	mDeviceId=devId;
	mDeviceName=devName;
	mSensor=sensor;
	mStoredValuesType=storedValType;
	mTimestampRule=tsRule;
	mStoreMode=stMode;
}

const SensorDef& ISensorStorage::sensor()const
{
	return mSensor;
}

QUuid ISensorStorage::deviceId()
{
	return mDeviceId;
}

QByteArray ISensorStorage::deviceName()
{
	return mDeviceName;
}

QByteArray ISensorStorage::storeModeToString(ISensorStorage::StoreMode mode)
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

ISensorStorage::StoreMode ISensorStorage::storeModeFromString(const QByteArray &str)
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

QByteArray ISensorStorage::timestampRuleToString(ISensorStorage::TimestampRule rule)
{
	if(rule==ADD_GT)
		return "add_global_time";
	else if(rule==DROP_TIME)
		return "drop_time";
	else /*DONT_TOUCH*/
		return "dont_touch";
}

bool ISensorStorage::timestampRuleFromString(
	const QByteArray &str,ISensorStorage::TimestampRule &rule)
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

ISensorStorage::TimestampRule ISensorStorage::timestampRule()const
{
	return mTimestampRule;
}

SensorDef::Type ISensorStorage::storedValuesType()const
{
	return mStoredValuesType;
}

ISensorStorage::StoreMode ISensorStorage::storeMode()const
{
	return mStoreMode;
}

bool ISensorStorage::values(quint64 index,quint64 count,quint64 step,VeryBigArray<SensorValue*> &vals)
{
	if(!isOpened())
		return false;
	vals.clear();
	for(quint64 i=0;i<count;++i)
	{
		SensorValue *v=valueAt(index+i*step);
		if(v)vals.append(v);
		else break;
	}
	return true;
}

bool ISensorStorage::hasGTIndex()
{
	return false;
}

quint64 ISensorStorage::findInGTIndex(qint64 ts)
{
	return 0ULL;
}

SensorDef::Type ISensorStorage::defaultStoredValuesType(
	SensorDef::Type sType,ISensorStorage::TimestampRule rule)
{
	SensorDef::Type retVal=sType;
	if(rule==ADD_GT)
		retVal.tsType=SensorDef::GLOBAL_TIME;
	else if(rule==DROP_TIME)
		retVal.tsType=SensorDef::NO_TIME;
	return retVal;
}
