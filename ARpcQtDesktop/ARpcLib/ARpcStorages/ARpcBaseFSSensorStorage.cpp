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

#include "ARpcBaseFSSensorStorage.h"
#include "ARpcAllStorages.h"
#include <QDir>
#include <QSettings>

ARpcBaseFSSensorStorage::ARpcBaseFSSensorStorage(const QUuid &devId,const QByteArray &devName,ARpcSensorDef sensor,
	StoreMode md,TimestampRule tsRule,QObject *parent)
	:ARpcISensorStorage(devId,devName,sensor,md,tsRule,defaultStoredValuesType(sensor.type,tsRule),parent)
{
	dbDirSet=false;
	hlp=ARpcDBDriverHelpers(tsRule);
}

ARpcBaseFSSensorStorage* ARpcBaseFSSensorStorage::preCreate(const QString &path,const QUuid &devId,
	const QByteArray &devName,ARpcSensorDef sensor,StoreMode mode,TimestampRule rule)
{
	//CRIT process fs errors
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())
		return 0;
	QDir dir(path);
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());

	ARpcBaseFSSensorStorage *st=makeStorage(devId,devName,sensor,mode,rule);
	if(!st)return 0;
	st->dbDir=dir;
	st->dbDirSet=true;

	QSettings file(dir.absolutePath()+"/"+settingsFileName(),QSettings::IniFormat);
	file.setValue("mode",QString::fromUtf8(storeModeToString(mode)));
	file.setValue("device_id",devId);
	file.setValue("device_name",QString::fromUtf8(devName));
	file.setValue("value_type",QString::fromUtf8(sensor.type.toString()));
	file.setValue("sensor_name",QString::fromUtf8(sensor.name));
	file.setValue("sensor_title",QString::fromUtf8(sensor.title));
	file.setValue("sensor_unit",QString::fromUtf8(sensor.unit));
	file.setValue("time_rule",QString::fromUtf8(timestampRuleToString(st->mTimestampRule)));

	file.beginGroup("sensor_attributes");
	for(auto i=sensor.attributes.begin();i!=sensor.attributes.end();++i)
		file.setValue(QString::fromUtf8(i.key()),QString::fromUtf8(i.value()));
	file.endGroup();

	file.sync();

	return st;
}

ARpcBaseFSSensorStorage* ARpcBaseFSSensorStorage::preOpen(const QString &path)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())
		return 0;
	QDir dir=QDir(path);
	if(!dir.exists())
		return 0;
	QSettings file(dir.absolutePath()+"/"+settingsFileName(),QSettings::IniFormat);
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

	file.beginGroup("sensor_attributes");
	for(const QString &k:file.allKeys())
		sensor.attributes[k.toUtf8()]=file.value(k).toString().toUtf8();
	file.endGroup();

	ARpcBaseFSSensorStorage *st=makeStorage(devId,devName,sensor,mode,rule);
	if(!st)return 0;
	st->dbDir=dir;
	st->dbDirSet=true;
	return st;
}

QDir ARpcBaseFSSensorStorage::getDbDir()const
{
	return dbDir;
}

void ARpcBaseFSSensorStorage::setDeviceName(const QByteArray &name)
{
	QSettings file(dbDir.absolutePath()+"/"+settingsFileName(),QSettings::IniFormat);
	file.setValue("device_name",QString::fromUtf8(name));
	mDeviceName=name;
}

bool ARpcBaseFSSensorStorage::isDbDirSet() const
{
	return dbDirSet;
}

void ARpcBaseFSSensorStorage::close()
{
	closeFS();
	dbDir=QDir();
	dbDirSet=false;
}

void ARpcBaseFSSensorStorage::writeAttribute(const QByteArray &str,const QVariant &var)
{
	if(!dbDirSet)
		return;
	QSettings file(dbDir.absolutePath()+"/"+settingsFileName(),QSettings::IniFormat);
	file.beginGroup("attributes");
	file.setValue(QString::fromUtf8(str),var);
	file.endGroup();
	file.sync();
}

QVariant ARpcBaseFSSensorStorage::readAttribute(const QByteArray &str)
{
	if(!dbDirSet)
		return QVariant();
	QSettings file(dbDir.absolutePath()+"/"+settingsFileName(),QSettings::IniFormat);
	file.beginGroup("attributes");
	return file.value(QString::fromUtf8(str));
}

bool ARpcBaseFSSensorStorage::hasDataExportConfig(const QByteArray &serviceType)
{
	if(!dbDirSet)return false;
	QSettings file(dbDir.absolutePath()+"/"+settingsFileName(),QSettings::IniFormat);
	if(!file.childGroups().contains(serviceType))return false;
	file.beginGroup(serviceType);
	return !file.allKeys().isEmpty();
}

ARpcBaseFSSensorStorage::DataExportConfig ARpcBaseFSSensorStorage::getDataExportConfig(const QByteArray &serviceType)
{
	if(!dbDirSet)return DataExportConfig();
	QSettings file(dbDir.absolutePath()+"/"+exportSettingsFileName(),QSettings::IniFormat);
	file.beginGroup(serviceType);
	DataExportConfig cfg;
	for(QString &k:file.allKeys())
		cfg[k.toUtf8()]=file.value(k).toByteArray();
	return cfg;
}

void ARpcBaseFSSensorStorage::removeDataExportConfig(const QByteArray &serviceType)
{
	if(!dbDirSet)return;
	QSettings file(dbDir.absolutePath()+"/"+exportSettingsFileName(),QSettings::IniFormat);
	file.beginGroup(serviceType);
	file.remove("");
}

QByteArrayList ARpcBaseFSSensorStorage::allDataExportServices()
{
	if(!dbDirSet)return QByteArrayList();
	QSettings file(dbDir.absolutePath()+"/"+exportSettingsFileName(),QSettings::IniFormat);
	QByteArrayList retVal;
	for(QString &s:file.childGroups())
		retVal.append(s.toUtf8());
	return retVal;
}

void ARpcBaseFSSensorStorage::addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)
{
	if(!dbDirSet)return;
	QSettings file(dbDir.absolutePath()+"/"+exportSettingsFileName(),QSettings::IniFormat);
	file.beginGroup(serviceType);
	file.remove("");
	for(auto i=cfg.begin();i!=cfg.end();++i)
		file.setValue(QString::fromUtf8(i.key()),i.value());
}

QString ARpcBaseFSSensorStorage::settingsFileName()
{
	return "database.ini";
}

QString ARpcBaseFSSensorStorage::exportSettingsFileName()
{
	return "data_export.ini";
}

ARpcSensorDef::Type ARpcBaseFSSensorStorage::defaultStoredValuesType(
	ARpcSensorDef::Type sType,ARpcBaseFSSensorStorage::TimestampRule rule)
{
	ARpcSensorDef::Type retVal=sType;
	if(rule==ADD_GT)
		retVal.tsType=ARpcSensorDef::GLOBAL_TIME;
	else if(rule==DROP_TIME)
		retVal.tsType=ARpcSensorDef::NO_TIME;
	return retVal;
}

ARpcBaseFSSensorStorage* ARpcBaseFSSensorStorage::makeStorage(const QUuid &devId,
	const QByteArray &devName,const ARpcSensorDef &sensor,ARpcBaseFSSensorStorage::StoreMode mode,TimestampRule rule)
{
	if(mode==CONTINUOUS)
		return new ARpcContinuousStorage(devId,devName,sensor,rule);
	else if(mode==MANUAL_SESSIONS)
		return new ARpcSessionStorage(false,devId,devName,sensor,rule);
	else if(mode==AUTO_SESSIONS)
		return new ARpcSessionStorage(true,devId,devName,sensor,rule);
	else if(mode==LAST_N_VALUES)
		return new ARpcLastNValuesStorage(devId,devName,sensor,rule);
	else if(mode==LAST_N_VALUES_IN_MEMORY)
		return new ARpcLastNValuesInMemoryStorage(devId,devName,sensor,rule);
	else
		return 0;
}
