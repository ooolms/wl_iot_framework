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

#ifndef BASEFSSENSORSTORAGE_H
#define BASEFSSENSORSTORAGE_H

#include "wliot/devices/SensorDef.h"
#include "wliot/devices/SensorValue.h"
#include "wliot/storages/ISensorStorage.h"
#include <QObject>
#include <QDir>
#include <QSettings>
#include <QUuid>
#include <QVariantMap>

class DBDriverHelpers;
class FSSensorStorageHelper;

class BaseFSSensorStorage
	:public ISensorStorage
{
	Q_OBJECT
protected:
	explicit BaseFSSensorStorage(const QString &path,
		const QUuid &devId,const QByteArray &devName,SensorDef sensor,StoreMode md,
		TimestampRule tsRule,QObject *parent=0);
	virtual ~BaseFSSensorStorage();

public:
	virtual void writeAttribute(const QByteArray &str,const QByteArray &var)override;
	virtual QByteArray readAttribute(const QByteArray &str)override;
	virtual void addDataExportConfig(const QUuid &serviceId,const DataExportConfig &cfg)override;
	virtual bool hasDataExportConfig(const QUuid &serviceId)override;
	virtual DataExportConfig getDataExportConfig(const QUuid &serviceId)override;
	virtual void removeDataExportConfig(const QUuid &serviceId)override;
	virtual QList<QUuid> allDataExportServices()override;

public:
	QDir getDbDir()const;
	void setDeviceName(const QByteArray &name);
	bool isDbDirSet()const;

protected:
	DBDriverHelpers *hlp;
	FSSensorStorageHelper *fsStorageHelper;
};

#endif // BASEFSSENSORSTORAGE_H
