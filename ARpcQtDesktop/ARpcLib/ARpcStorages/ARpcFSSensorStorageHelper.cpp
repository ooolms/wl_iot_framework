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

#include "ARpcFSSensorStorageHelper.h"
#include "ARpcAllStorages.h"
#include <QDir>
#include <QSettings>

ARpcFSSensorStorageHelper::ARpcFSSensorStorageHelper(const QString &path,QObject *parent)
	:QObject(parent)
{
	QDir dir(path);
	if(dir.exists())
		mDbPath=dir.absolutePath();
	else mDbPath.clear();
	if(mDbPath.isEmpty())
	{
		mSettings=new QSettings("/",QSettings::IniFormat);
		mExportSettings=new QSettings("/",QSettings::IniFormat);
	}
	else
	{
		mSettings=new QSettings(mDbPath+"/database.ini",QSettings::IniFormat);
		mExportSettings=new QSettings(mDbPath+"/data_export.ini",QSettings::IniFormat);
	}
}

ARpcFSSensorStorageHelper::~ARpcFSSensorStorageHelper()
{
	delete mExportSettings;
	delete mSettings;
}

QString ARpcFSSensorStorageHelper::dbPath()
{
	return mDbPath;
}

void ARpcFSSensorStorageHelper::writeAttribute(const QByteArray &str,const QByteArray &var)
{
	if(mDbPath.isEmpty())
		return;
	mSettings->beginGroup("attributes");
	mSettings->setValue(QString::fromUtf8(str),QString::fromUtf8(var));
	mSettings->endGroup();
	mSettings->sync();
}

QByteArray ARpcFSSensorStorageHelper::readAttribute(const QByteArray &str)
{
	if(mDbPath.isEmpty())
		return QByteArray();
	mSettings->beginGroup("attributes");
	QByteArray v=mSettings->value(QString::fromUtf8(str)).toString().toUtf8();
	mSettings->endGroup();
	return v;
}

void ARpcFSSensorStorageHelper::addDataExportConfig(
	const QByteArray &serviceType,const ARpcISensorStorage::DataExportConfig &cfg)
{
	if(mDbPath.isEmpty())return;
	mExportSettings->beginGroup(serviceType);
	mExportSettings->remove("");
	for(auto i=cfg.begin();i!=cfg.end();++i)
		mExportSettings->setValue(QString::fromUtf8(i.key()),i.value());
	mExportSettings->endGroup();
}

bool ARpcFSSensorStorageHelper::hasDataExportConfig(const QByteArray &serviceType)
{
	if(mDbPath.isEmpty())
		return false;
	if(!mExportSettings->childGroups().contains(serviceType))return false;
	mExportSettings->beginGroup(serviceType);
	bool has=!mExportSettings->allKeys().isEmpty();
	mExportSettings->endGroup();
	return has;
}

ARpcISensorStorage::DataExportConfig ARpcFSSensorStorageHelper::getDataExportConfig(const QByteArray &serviceType)
{
	if(mDbPath.isEmpty())
		return ARpcISensorStorage::DataExportConfig();
	mExportSettings->beginGroup(serviceType);
	ARpcISensorStorage::DataExportConfig cfg;
	for(QString &k:mExportSettings->allKeys())
		cfg[k.toUtf8()]=mExportSettings->value(k).toByteArray();
	mExportSettings->endGroup();
	return cfg;
}

void ARpcFSSensorStorageHelper::removeDataExportConfig(const QByteArray &serviceType)
{
	if(mDbPath.isEmpty())return;
	mExportSettings->beginGroup(serviceType);
	mExportSettings->remove("");
}

QByteArrayList ARpcFSSensorStorageHelper::allDataExportServices()
{
	if(mDbPath.isEmpty())
		return QByteArrayList();
	QByteArrayList retVal;
	for(QString &s:mExportSettings->childGroups())
		retVal.append(s.toUtf8());
	return retVal;
}

ARpcISensorStorage* ARpcFSSensorStorageHelper::preCreate(const QString &path,const QUuid &devId,
	const QByteArray &devName,ARpcSensorDef sensor,ARpcISensorStorage::StoreMode mode,
	ARpcISensorStorage::TimestampRule rule)
{
	//CRIT process fs errors
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())
		return 0;
	QDir dir(path);
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());

	{
		QSettings file(dir.absolutePath()+"/database.ini",QSettings::IniFormat);
		file.setValue("mode",QString::fromUtf8(ARpcISensorStorage::storeModeToString(mode)));
		file.setValue("device_id",devId);
		file.setValue("device_name",QString::fromUtf8(devName));
		file.setValue("value_type",QString::fromUtf8(sensor.type.toString()));
		file.setValue("sensor_name",QString::fromUtf8(sensor.name));
		file.setValue("sensor_title",QString::fromUtf8(sensor.title));
		file.setValue("sensor_unit",QString::fromUtf8(sensor.unit));
		file.setValue("time_rule",QString::fromUtf8(ARpcISensorStorage::timestampRuleToString(rule)));

		file.beginGroup("sensor_attributes");
		for(auto i=sensor.attributes.begin();i!=sensor.attributes.end();++i)
			file.setValue(QString::fromUtf8(i.key()),QString::fromUtf8(i.value()));
		file.endGroup();

		file.sync();
	}

	return makeStorage(path,devId,devName,sensor,mode,rule);
}

ARpcISensorStorage* ARpcFSSensorStorageHelper::preOpen(const QString &path)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())
		return 0;
	QDir dir=QDir(path);
	if(!dir.exists())
		return 0;
	ARpcSensorDef sensor;
	QUuid devId;
	QByteArray devName;
	ARpcISensorStorage::StoreMode mode;
	ARpcISensorStorage::TimestampRule rule;
	{
		QSettings file(dir.absolutePath()+"/database.ini",QSettings::IniFormat);
		mode=ARpcISensorStorage::storeModeFromString(file.value("mode").toString().toUtf8());
		if(mode==ARpcISensorStorage::BAD_MODE)
			return 0;
		sensor.name=file.value("sensor_name").toString().toUtf8();
		if(!sensor.type.fromString(file.value("value_type").toString().toUtf8())||
			!sensor.type.isValid()||sensor.name.isEmpty())
			return 0;
		sensor.title=file.value("sensor_title").toString().toUtf8();
		sensor.unit=file.value("sensor_unit").toString().toUtf8();
		if(sensor.name.isEmpty())
			return 0;
		devId=file.value("device_id").toUuid();
		devName=file.value("device_name").toString().toUtf8();
		if(devId.isNull())
			return 0;
		if(!ARpcISensorStorage::timestampRuleFromString(file.value("time_rule").toString().toUtf8(),rule))
			return 0;

		file.beginGroup("sensor_attributes");
		for(const QString &k:file.allKeys())
			sensor.attributes[k.toUtf8()]=file.value(k).toString().toUtf8();
		file.endGroup();
	}

	return makeStorage(path,devId,devName,sensor,mode,rule);
}

void ARpcFSSensorStorageHelper::storeDeviceName(const QByteArray &name)
{
	if(mDbPath.isEmpty())
		return;
	mSettings->setValue("device_name",QString::fromUtf8(name));
	mSettings->sync();
}

QSettings* ARpcFSSensorStorageHelper::settings()
{
	return mSettings;
}

ARpcISensorStorage *ARpcFSSensorStorageHelper::makeStorage(
	const QString &path,const QUuid &devId,const QByteArray &devName,
	const ARpcSensorDef &sensor,ARpcISensorStorage::StoreMode mode,ARpcISensorStorage::TimestampRule rule)
{
	if(mode==ARpcISensorStorage::CONTINUOUS)
		return new ARpcContinuousStorage(path,devId,devName,sensor,rule);
	else if(mode==ARpcISensorStorage::MANUAL_SESSIONS)
		return new ARpcSessionStorage(path,false,devId,devName,sensor,rule);
	else if(mode==ARpcISensorStorage::AUTO_SESSIONS)
		return new ARpcSessionStorage(path,true,devId,devName,sensor,rule);
	else if(mode==ARpcISensorStorage::LAST_N_VALUES)
		return new ARpcLastNValuesStorage(path,devId,devName,sensor,rule);
	else if(mode==ARpcISensorStorage::LAST_N_VALUES_IN_MEMORY)
		return new ARpcLastNValuesInMemoryStorage(path,devId,devName,sensor,rule);
	else
		return 0;
}
