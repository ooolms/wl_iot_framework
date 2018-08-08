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

ARpcISensorStorage::ARpcISensorStorage(ARpcSensorDef sensor,const QUuid &devId,const QByteArray &devName,QObject *parent)
	:QObject(parent)
{
	mSensor=sensor;
	mDeviceId=devId;
	mDeviceName=devName;
	dbDirSet=false;
	timestampRule=DONT_TOUCH;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
}

ARpcISensorStorage* ARpcISensorStorage::preCreate(const QString &path,ARpcISensorStorage::StoreMode mode,
	ARpcSensorDef sensor,const QUuid &devId,const QByteArray &devName,TimestampRule rule)
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
	st->timestampRule=rule;
	st->effectiveValType=st->defaultEffectiveValuesType(st->timestampRule);

	QSettings file(dir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	file.setValue("mode",QString::fromUtf8(storeModeToString(mode)));
	file.setValue("device_id",devId);
	file.setValue("device_name",QString::fromUtf8(devName));
	file.setValue("value_type",QString::fromUtf8(sensor.type.toString()));
	file.setValue("sensor_name",QString::fromUtf8(sensor.name));
	file.setValue("sensor_title",QString::fromUtf8(sensor.title));
	file.setValue("sensor_unit",QString::fromUtf8(sensor.unit));
	file.setValue("time_rule",QString::fromUtf8(timestampRuleToString(st->timestampRule)));

	file.beginGroup("sensor_attributes");
	for(auto i=sensor.attributes.begin();i!=sensor.attributes.end();++i)
		file.setValue(QString::fromUtf8(i.key()),QString::fromUtf8(i.value()));
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
	StoreMode mode=storeModeFromString(file.value("mode").toString().toUtf8());
	if(mode==BAD_MODE)
		return 0;
	ARpcSensorDef sensor;
	sensor.name=file.value("sensor_name").toString().toUtf8();
	if(!sensor.type.fromString(file.value("value_type").toString().toUtf8())||
		!sensor.type.isValid()||sensor.name.isEmpty())
		return 0;
	sensor.title=file.value("sensor_title").toString().toUtf8();
	sensor.unit=file.value("sensor_unit").toString().toUtf8();
	if(sensor.name.isEmpty())
		return 0;
	QUuid devId=file.value("device_id").toUuid();
	QByteArray devName=file.value("device_name").toString().toUtf8();
	if(devId.isNull())
		return 0;
	TimestampRule rule;
	if(!timestampRuleFromString(file.value("time_rule").toString().toUtf8(),rule))
		return 0;

	file.beginGroup("sensor_constraints");
	for(const QString &k:file.allKeys())
		sensor.attributes[k.toUtf8()]=file.value(k).toString().toUtf8();
	file.endGroup();

	ARpcISensorStorage *st=makeStorage(sensor,devId,devName,mode);
	if(!st)
		return 0;
	st->dbDir=dir;
	st->dbDirSet=true;
	st->timestampRule=rule;
	st->effectiveValType=st->defaultEffectiveValuesType(st->timestampRule);
	return st;
}

const ARpcSensorDef& ARpcISensorStorage::sensor()const
{
	return mSensor;
}

QDir ARpcISensorStorage::getDbDir()const
{
	return dbDir;
}

QUuid ARpcISensorStorage::deviceId()
{
	return mDeviceId;
}

QByteArray ARpcISensorStorage::deviceName()
{
	return mDeviceName;
}

void ARpcISensorStorage::setDeviceName(const QByteArray &name)
{
	QSettings file(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	file.setValue("device_name",QString::fromUtf8(name));
	mDeviceName=name;
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
	else if(mode==LAST_VALUE_IN_MEMORY)
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

ARpcSensorDef::Type ARpcISensorStorage::effectiveValuesType() const
{
	return effectiveValType;
}

void ARpcISensorStorage::close()
{
	closeInternal();
	dbDir=QDir();
	dbDirSet=false;
	mSensor.type.numType=ARpcSensorDef::BAD_TYPE;
	mSensor.name.clear();
	mSensor.attributes.clear();
}

void ARpcISensorStorage::writeAttribute(const QByteArray &str,const QVariant &var)
{
	if(!dbDirSet)
		return;
	QSettings file(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	file.beginGroup("attributes");
	file.setValue(QString::fromUtf8(str),var);
	file.endGroup();
	file.sync();
}

QVariant ARpcISensorStorage::readAttribute(const QByteArray &str)
{
	if(!dbDirSet)
		return QVariant();
	QSettings file(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	file.beginGroup("attributes");
	return file.value(QString::fromUtf8(str));
}

QString ARpcISensorStorage::settingsFileRelPath()
{
	return "database.ini";
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

bool ARpcISensorStorage::timestampRuleFromString(const QByteArray &str,ARpcISensorStorage::TimestampRule &rule)
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

ARpcSensorDef::Type ARpcISensorStorage::defaultEffectiveValuesType(ARpcISensorStorage::TimestampRule rule)
{
	ARpcSensorDef::Type retVal=mSensor.type;
	if(rule==ADD_GT)
		retVal.tsType=ARpcSensorDef::GLOBAL_TIME;
	else if(rule==DROP_TIME)
		retVal.tsType=ARpcSensorDef::NO_TIME;
	return retVal;
}

ARpcISensorStorage* ARpcISensorStorage::makeStorage(const ARpcSensorDef &sensor,const QUuid &devId,
	const QByteArray &devName,ARpcISensorStorage::StoreMode mode)
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
