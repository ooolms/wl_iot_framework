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

#include "ARpcISensorStorage.h"
#include "ARpcAllStorages.h"
#include <QDir>
#include <QSettings>

ARpcISensorStorage::ARpcISensorStorage(ARpcSensor sensor,const QUuid &devId,const QString &devName,QObject *parent)
	:QObject(parent)
{
	mSensor=sensor;
	mDeviceId=devId;
	mDeviceName=devName;
	dbDirSet=false;
	timestampRule=DONT_TOUCH;
}

ARpcISensorStorage* ARpcISensorStorage::preCreate(const QString &path,ARpcISensorStorage::StoreMode mode,
	ARpcSensor sensor,const QUuid &devId,const QString &devName,
	TimestampRule rule)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())
		return 0;
	QDir dir(path);
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());
	ARpcISensorStorage *st=makeStorage(sensor,devId,devName,mode);
	st->dbDir=dir;
	st->dbDirSet=true;
	st->timestampRule=st->fixTimestampRule(rule);

	QSettings file(dir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	file.setValue("mode",storeModeToString(mode));
	file.setValue("device_id",devId);
	file.setValue("device_name",devName);
	file.setValue("value_type",ARpcSensor::typeToString(sensor.type));
	file.setValue("sensor_name",sensor.name);
	file.setValue("time_rule",timestampRuleToString(st->timestampRule));

	file.beginGroup("sensor_constraints");
	for(auto i=sensor.constraints.begin();i!=sensor.constraints.end();++i)
		file.setValue(i.key(),i.value());
	file.endGroup();

	file.sync();

	return st;
}

ARpcISensorStorage* ARpcISensorStorage::preOpen(const QString &path)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())
		return 0;
	QDir dir=QDir(path);
	if(!dir.exists())
		return 0;
	QSettings file(dir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	StoreMode mode=storeModeFromString(file.value("mode").toString());
	if(mode==BAD_MODE)
		return 0;
	ARpcSensor sensor;
	sensor.type=ARpcSensor::typeFromString(file.value("value_type").toString());
	sensor.name=file.value("sensor_name").toString();
	if(sensor.type==ARpcSensor::BAD_TYPE||sensor.name.isEmpty())
		return 0;
	QUuid devId=file.value("device_id").toUuid();
	QString devName=file.value("device_name").toString();
	if(devId.isNull())
		return 0;
	TimestampRule rule;
	if(!timestampRuleFromString(file.value("time_rule").toString(),rule))
		return 0;

	file.beginGroup("sensor_constraints");
	for(const QString &k:file.allKeys())
		sensor.constraints[k]=file.value(k).toString();
	file.endGroup();

	ARpcISensorStorage *st=makeStorage(sensor,devId,devName,mode);
	if(!st)
		return 0;
	st->dbDir=dir;
	st->dbDirSet=true;
	st->timestampRule=st->fixTimestampRule(rule);
	return st;
}

const ARpcSensor& ARpcISensorStorage::sensor() const
{
	return mSensor;
}

QDir ARpcISensorStorage::getDbDir() const
{
	return dbDir;
}

QUuid ARpcISensorStorage::deviceId()
{
	return mDeviceId;
}

QString ARpcISensorStorage::deviceName()
{
	return mDeviceName;
}

QString ARpcISensorStorage::storeModeToString(ARpcISensorStorage::StoreMode mode)
{
	if(mode==CONTINUOUS)
		return "continuous";
	else if(mode==MANUAL_SESSIONS)
		return "manual_sessions";
	else if(mode==AUTO_SESSIONS)
		return "auto_sessions";
	else if(mode==LAST_N_VALUES)
		return "last_n_values";
	else if(mode==LAST_VALUE_IN_MEMORY)
		return "memory";
	else
		return QString();
}

ARpcISensorStorage::StoreMode ARpcISensorStorage::storeModeFromString(const QString &str)
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
		return LAST_VALUE_IN_MEMORY;
	else
		return BAD_MODE;
}

bool ARpcISensorStorage::isDbDirSet() const
{
	return dbDirSet;
}

ARpcISensorStorage::TimestampRule ARpcISensorStorage::getTimestampRule() const
{
	return timestampRule;
}

void ARpcISensorStorage::close()
{
	closeInternal();
	dbDir=QDir();
	dbDirSet=false;
	mSensor.type=ARpcSensor::BAD_TYPE;
	mSensor.name.clear();
	mSensor.constraints.clear();
}

void ARpcISensorStorage::writeAttribute(const QString &str,const QVariant &var)
{
	if(!dbDirSet)
		return;
	QSettings file(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	file.beginGroup("attributes");
	file.setValue(str,var);
	file.endGroup();
	file.sync();
}

QVariant ARpcISensorStorage::readAttribute(const QString &str)
{
	if(!dbDirSet)
		return QVariant();
	QSettings file(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	file.beginGroup("attributes");
	return file.value(str);
}

QString ARpcISensorStorage::settingsFileRelPath()
{
	return "database.ini";
}

QString ARpcISensorStorage::timestampRuleToString(ARpcISensorStorage::TimestampRule rule)
{
	if(rule==ADD_GT)
		return "add_global_time";
	else if(rule==DROP_TIME)
		return "drop_time";
	else /*DONT_TOUCH*/
		return "dont_touch";
}

bool ARpcISensorStorage::timestampRuleFromString(const QString &str,ARpcISensorStorage::TimestampRule &rule)
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

ARpcSensor::Type ARpcISensorStorage::defaultEffectiveValuesType(ARpcISensorStorage::TimestampRule rule)
{
	if(rule==ADD_GT)
	{
		if(mSensor.type==ARpcSensor::SINGLE||mSensor.type==ARpcSensor::SINGLE_LT)
			return ARpcSensor::SINGLE_GT;
		else if(mSensor.type==ARpcSensor::PACKET||mSensor.type==ARpcSensor::PACKET_LT)
			return ARpcSensor::PACKET_GT;
		else
			return mSensor.type;
	}
	else if(rule==DROP_TIME)
	{
		if(mSensor.type==ARpcSensor::SINGLE_LT||mSensor.type==ARpcSensor::SINGLE_GT)
			return ARpcSensor::SINGLE;
		else if(mSensor.type==ARpcSensor::PACKET_LT||mSensor.type==ARpcSensor::PACKET_GT)
			return ARpcSensor::PACKET;
		else
			return mSensor.type;
	}
	else /*rule==DONT_TOUCH*/
		return mSensor.type;
}

ARpcISensorStorage* ARpcISensorStorage::makeStorage(const ARpcSensor &sensor,const QUuid &devId,const QString &devName,
	ARpcISensorStorage::StoreMode mode)
{
	if(mode==CONTINUOUS)
		return new ARpcContinuousStorage(sensor,devId,devName);
	else if(mode==MANUAL_SESSIONS)
		return new ARpcSessionStorage(false,sensor,devId,devName);
	else if(mode==AUTO_SESSIONS)
		return new ARpcSessionStorage(true,sensor,devId,devName);
	else if(mode==LAST_N_VALUES)
		return new ARpcLastNValuesStorage(sensor,devId,devName);
	else if(mode==LAST_VALUE_IN_MEMORY)
		return new ARpcLastValueInMemoryStorage(sensor,devId,devName);
	else
		return 0;
}
