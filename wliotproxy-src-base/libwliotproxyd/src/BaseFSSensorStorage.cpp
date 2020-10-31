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

using namespace WLIOT;

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

bool BaseFSSensorStorage::hasDataExportConfig(const QUuid &serviceId)
{
	return fsStorageHelper->hasDataExportConfig(serviceId);
}

BaseFSSensorStorage::DataExportConfig BaseFSSensorStorage::getDataExportConfig(const QUuid &serviceId)
{
	return fsStorageHelper->getDataExportConfig(serviceId);
}

void BaseFSSensorStorage::removeDataExportConfig(const QUuid &serviceId)
{
	fsStorageHelper->removeDataExportConfig(serviceId);
}

QList<QUuid> BaseFSSensorStorage::allDataExportServices()
{
	return fsStorageHelper->allDataExportServices();
}

SensorValue* BaseFSSensorStorage::lastValue()
{
	return valueAt(valuesCount()-1);
}

void BaseFSSensorStorage::addDataExportConfig(const QUuid &serviceId,const DataExportConfig &cfg)
{
	fsStorageHelper->addDataExportConfig(serviceId,cfg);
}
