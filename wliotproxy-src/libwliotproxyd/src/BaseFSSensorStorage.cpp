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

#include "wliot/storages/BaseFSSensorStorage.h"
#include "wliot/storages/AllStorages.h"
#include "DBDriverHelpers.h"
#include "FSSensorStorageHelper.h"
#include <QDir>
#include <QSettings>

BaseFSSensorStorage::BaseFSSensorStorage(const QString &path,
	const QUuid &devId,const QByteArray &devName,SensorDef sensor,StoreMode md,TimestampRule tsRule,QObject *parent)
	:ISensorStorage(devId,devName,sensor,md,tsRule,defaultStoredValuesType(sensor.type,tsRule),parent)
{
	hlp=new DBDriverHelpers(tsRule);
	fsStorageHelper=new FSSensorStorageHelper(path);
}

BaseFSSensorStorage::~BaseFSSensorStorage()
{
	delete fsStorageHelper;
	delete hlp;
}

void BaseFSSensorStorage::setDeviceName(const QByteArray &name)
{
	mDeviceName=name;
	fsStorageHelper->storeDeviceName(name);
}

void BaseFSSensorStorage::writeAttribute(const QByteArray &str,const QByteArray &var)
{
	fsStorageHelper->writeAttribute(str,var);
}

QByteArray BaseFSSensorStorage::readAttribute(const QByteArray &str)
{
	return fsStorageHelper->readAttribute(str);
}

bool BaseFSSensorStorage::hasDataExportConfig(const QByteArray &serviceType)
{
	return fsStorageHelper->hasDataExportConfig(serviceType);
}

BaseFSSensorStorage::DataExportConfig BaseFSSensorStorage::getDataExportConfig(const QByteArray &serviceType)
{
	return fsStorageHelper->getDataExportConfig(serviceType);
}

void BaseFSSensorStorage::removeDataExportConfig(const QByteArray &serviceType)
{
	fsStorageHelper->removeDataExportConfig(serviceType);
}

QByteArrayList BaseFSSensorStorage::allDataExportServices()
{
	return fsStorageHelper->allDataExportServices();
}

void BaseFSSensorStorage::addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)
{
	fsStorageHelper->addDataExportConfig(serviceType,cfg);
}
