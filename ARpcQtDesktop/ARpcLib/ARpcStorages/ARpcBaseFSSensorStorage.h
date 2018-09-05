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
	explicit ARpcBaseFSSensorStorage(
		const QUuid &devId,const QByteArray &devName,ARpcSensorDef sensor,StoreMode md,
		TimestampRule tsRule,QObject *parent=0);

public:
	virtual void close()override;
	virtual void writeAttribute(const QByteArray &str,const QVariant &var)override;
	virtual QVariant readAttribute(const QByteArray &str)override;
	virtual void addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)override;
	virtual bool hasDataExportConfig(const QByteArray &serviceType)override;
	virtual DataExportConfig getDataExportConfig(const QByteArray &serviceType)override;
	virtual void removeDataExportConfig(const QByteArray &serviceType)override;
	virtual QByteArrayList allDataExportServices()override;

public:
	static ARpcBaseFSSensorStorage* preCreate(const QString &path,const QUuid &devId,
		const QByteArray &devName,ARpcSensorDef sensor,StoreMode mStoreMode,TimestampRule rule);
	//не создает саму базу, только создает папку и сохраняет mode и valueType
	static ARpcBaseFSSensorStorage* preOpen(const QString &path);
	QDir getDbDir()const;
	void setDeviceName(const QByteArray &name);
	bool isDbDirSet()const;

protected:
	virtual void closeFS()=0;
	static QString settingsFileName();
	static QString exportSettingsFileName();
	static ARpcSensorDef::Type defaultStoredValuesType(ARpcSensorDef::Type sType,TimestampRule rule);

private:
	static ARpcBaseFSSensorStorage* makeStorage(
		const QUuid &devId,const QByteArray &devName,const ARpcSensorDef &sensor,
		StoreMode mStoreMode,TimestampRule rule);

protected:
	QDir dbDir;
	bool dbDirSet;
	ARpcDBDriverHelpers hlp;
};

#endif // ARPCBASEFSSENSORSTORAGE_H
