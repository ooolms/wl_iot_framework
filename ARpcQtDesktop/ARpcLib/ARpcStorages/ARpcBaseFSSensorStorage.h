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

#ifndef ARPCBASEFSSENSORSTORAGE_H
#define ARPCBASEFSSENSORSTORAGE_H

#include "ARpcBase/ARpcSensorDef.h"
#include "ARpcBase/ARpcSensorValue.h"
#include "ARpcStorages/ARpcISensorStorage.h"
#include "ARpcStorages/ARpcDBDriverHelpers.h"
#include "ARpcFSSensorStorageHelper.h"
#include <QObject>
#include <QDir>
#include <QSettings>
#include <QUuid>
#include <QVariantMap>

class ARpcBaseFSSensorStorage
	:public ARpcISensorStorage
{
	Q_OBJECT
protected:
	explicit ARpcBaseFSSensorStorage(const QString &path,
		const QUuid &devId,const QByteArray &devName,ARpcSensorDef sensor,StoreMode md,
		TimestampRule tsRule,QObject *parent=0);

public:
	virtual void writeAttribute(const QByteArray &str,const QByteArray &var)override;
	virtual QByteArray readAttribute(const QByteArray &str)override;
	virtual void addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)override;
	virtual bool hasDataExportConfig(const QByteArray &serviceType)override;
	virtual DataExportConfig getDataExportConfig(const QByteArray &serviceType)override;
	virtual void removeDataExportConfig(const QByteArray &serviceType)override;
	virtual QByteArrayList allDataExportServices()override;

public:
	QDir getDbDir()const;
	void setDeviceName(const QByteArray &name);
	bool isDbDirSet()const;

protected:
	ARpcDBDriverHelpers hlp;
	ARpcFSSensorStorageHelper fsStorageHelper;
};

#endif // ARPCBASEFSSENSORSTORAGE_H
