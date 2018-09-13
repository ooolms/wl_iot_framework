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

ARpcBaseFSSensorStorage::ARpcBaseFSSensorStorage(const QString &path,
	const QUuid &devId,const QByteArray &devName,ARpcSensorDef sensor,StoreMode md,TimestampRule tsRule,QObject *parent)
	:ARpcISensorStorage(devId,devName,sensor,md,tsRule,defaultStoredValuesType(sensor.type,tsRule),parent)
	,fsStorageHelper(path)
{
	hlp=ARpcDBDriverHelpers(tsRule);
}

void ARpcBaseFSSensorStorage::setDeviceName(const QByteArray &name)
{
	mDeviceName=name;
	fsStorageHelper.storeDeviceName(name);
}

void ARpcBaseFSSensorStorage::writeAttribute(const QByteArray &str,const QByteArray &var)
{
	fsStorageHelper.writeAttribute(str,var);
}

QByteArray ARpcBaseFSSensorStorage::readAttribute(const QByteArray &str)
{
	return fsStorageHelper.readAttribute(str);
}

bool ARpcBaseFSSensorStorage::hasDataExportConfig(const QByteArray &serviceType)
{
	return fsStorageHelper.hasDataExportConfig(serviceType);
}

ARpcBaseFSSensorStorage::DataExportConfig ARpcBaseFSSensorStorage::getDataExportConfig(const QByteArray &serviceType)
{
	return fsStorageHelper.getDataExportConfig(serviceType);
}

void ARpcBaseFSSensorStorage::removeDataExportConfig(const QByteArray &serviceType)
{
	fsStorageHelper.removeDataExportConfig(serviceType);
}

QByteArrayList ARpcBaseFSSensorStorage::allDataExportServices()
{
	return fsStorageHelper.allDataExportServices();
}

void ARpcBaseFSSensorStorage::addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)
{
	fsStorageHelper.addDataExportConfig(serviceType,cfg);
}
