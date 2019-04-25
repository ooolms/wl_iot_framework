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

#include "FSSensorStorageHelper.h"
#include "wliot/storages/AllStorages.h"
#include <QDir>
#include <QSettings>

FSSensorStorageHelper::FSSensorStorageHelper(const QString &path)
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

FSSensorStorageHelper::~FSSensorStorageHelper()
{
	delete mExportSettings;
	delete mSettings;
}

QString FSSensorStorageHelper::dbPath()
{
	return mDbPath;
}

void FSSensorStorageHelper::writeAttribute(const QByteArray &str,const QByteArray &var)
{
	if(mDbPath.isEmpty())
		return;
	mSettings->beginGroup("attributes");
	mSettings->setValue(QString::fromUtf8(str),QString::fromUtf8(var));
	mSettings->endGroup();
	mSettings->sync();
}

QByteArray FSSensorStorageHelper::readAttribute(const QByteArray &str)
{
	if(mDbPath.isEmpty())
		return QByteArray();
	mSettings->beginGroup("attributes");
	QByteArray v=mSettings->value(QString::fromUtf8(str)).toString().toUtf8();
	mSettings->endGroup();
	return v;
}

void FSSensorStorageHelper::addDataExportConfig(const QUuid &serviceId,const ISensorStorage::DataExportConfig &cfg)
{
	if(mDbPath.isEmpty())return;
	mExportSettings->beginGroup(serviceId.toString());
	mExportSettings->remove("");
	for(auto i=cfg.begin();i!=cfg.end();++i)
		mExportSettings->setValue(QString::fromUtf8(i.key()),i.value());
	mExportSettings->endGroup();
}

bool FSSensorStorageHelper::hasDataExportConfig(const QUuid &serviceId)
{
	if(mDbPath.isEmpty())
		return false;
	if(!mExportSettings->childGroups().contains(serviceId.toString()))return false;
	mExportSettings->beginGroup(serviceId.toString());
	bool has=!mExportSettings->allKeys().isEmpty();
	mExportSettings->endGroup();
	return has;
}

ISensorStorage::DataExportConfig FSSensorStorageHelper::getDataExportConfig(const QUuid &serviceId)
{
	if(mDbPath.isEmpty())
		return ISensorStorage::DataExportConfig();
	mExportSettings->beginGroup(serviceId.toString());
	ISensorStorage::DataExportConfig cfg;
	for(QString &k:mExportSettings->allKeys())
		cfg[k.toUtf8()]=mExportSettings->value(k).toByteArray();
	mExportSettings->endGroup();
	return cfg;
}

void FSSensorStorageHelper::removeDataExportConfig(const QUuid &serviceId)
{
	if(mDbPath.isEmpty())return;
	mExportSettings->beginGroup(serviceId.toString());
	mExportSettings->remove("");
}

QList<QUuid> FSSensorStorageHelper::allDataExportServices()
{
	if(mDbPath.isEmpty())
		return QList<QUuid>();
	QList<QUuid> retVal;
	for(QString &s:mExportSettings->childGroups())
	{
		QUuid uid=QUuid(s);
		if(!uid.isNull())
			retVal.append(s);
	}
	return retVal;
}

ISensorStorage* FSSensorStorageHelper::preCreate(const QString &path,const QUuid &devId,
	const QByteArray &devName,SensorDef sensor,ISensorStorage::StoreMode mode,
	ISensorStorage::TimestampRule rule)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())
		return 0;
	QDir dir(path);
	if(!dir.exists())
		if(!dir.mkpath(dir.absolutePath()))return 0;

	{
		QSettings file(dir.absolutePath()+"/database.ini",QSettings::IniFormat);
		file.setValue("mode",QString::fromUtf8(ISensorStorage::storeModeToString(mode)));
		file.setValue("device_id",devId);
		file.setValue("device_name",QString::fromUtf8(devName));
		file.setValue("value_type",QString::fromUtf8(sensor.type.toString()));
		file.setValue("sensor_name",QString::fromUtf8(sensor.name));
		file.setValue("sensor_title",QString::fromUtf8(sensor.title));
		file.setValue("sensor_unit",QString::fromUtf8(sensor.unit));
		file.setValue("time_rule",QString::fromUtf8(ISensorStorage::timestampRuleToString(rule)));

		file.beginGroup("sensor_attributes");
		for(auto i=sensor.attributes.begin();i!=sensor.attributes.end();++i)
			file.setValue(QString::fromUtf8(i.key()),QString::fromUtf8(i.value()));
		file.endGroup();

		file.sync();
		if(file.status()!=QSettings::NoError)return 0;
	}

	return makeStorage(path,devId,devName,sensor,mode,rule);
}

ISensorStorage* FSSensorStorageHelper::preOpen(const QString &path)
{
	QFileInfo fInfo(path);
	if(fInfo.exists()&&!fInfo.isDir())
		return 0;
	QDir dir=QDir(path);
	if(!dir.exists())
		return 0;
	SensorDef sensor;
	QUuid devId;
	QByteArray devName;
	ISensorStorage::StoreMode mode;
	ISensorStorage::TimestampRule rule;
	{
		QSettings file(dir.absolutePath()+"/database.ini",QSettings::IniFormat);
		mode=ISensorStorage::storeModeFromString(file.value("mode").toString().toUtf8());
		if(mode==ISensorStorage::BAD_MODE)
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
		if(!ISensorStorage::timestampRuleFromString(file.value("time_rule").toString().toUtf8(),rule))
			return 0;

		file.beginGroup("sensor_attributes");
		for(const QString &k:file.allKeys())
			sensor.attributes[k.toUtf8()]=file.value(k).toString().toUtf8();
		file.endGroup();
	}

	return makeStorage(path,devId,devName,sensor,mode,rule);
}

void FSSensorStorageHelper::storeDeviceName(const QByteArray &name)
{
	if(mDbPath.isEmpty())
		return;
	mSettings->setValue("device_name",QString::fromUtf8(name));
	mSettings->sync();
}

QSettings* FSSensorStorageHelper::settings()
{
	return mSettings;
}

ISensorStorage *FSSensorStorageHelper::makeStorage(
	const QString &path,const QUuid &devId,const QByteArray &devName,
	const SensorDef &sensor,ISensorStorage::StoreMode mode,ISensorStorage::TimestampRule rule)
{
	if(mode==ISensorStorage::CONTINUOUS)
		return new ContinuousStorage(path,devId,devName,sensor,rule);
	else if(mode==ISensorStorage::MANUAL_SESSIONS)
		return new SessionStorage(path,false,devId,devName,sensor,rule);
	else if(mode==ISensorStorage::AUTO_SESSIONS)
		return new SessionStorage(path,true,devId,devName,sensor,rule);
	else if(mode==ISensorStorage::LAST_N_VALUES)
		return new LastNValuesStorage(path,devId,devName,sensor,rule);
	else if(mode==ISensorStorage::LAST_N_VALUES_IN_MEMORY)
		return new LastNValuesInMemoryStorage(path,devId,devName,sensor,rule);
	else
		return 0;
}
