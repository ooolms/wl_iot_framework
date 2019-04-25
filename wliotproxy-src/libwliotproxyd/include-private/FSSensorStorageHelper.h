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

#ifndef FSSENSORSTORAGEHELPER_H
#define FSSENSORSTORAGEHELPER_H

#include <QSettings>
#include "wliot/storages/ISensorStorage.h"

class FSSensorStorageHelper
{
public:
	explicit FSSensorStorageHelper(const QString &path);
	~FSSensorStorageHelper();
//	void setDBPath(const QString &path);
	QString dbPath();
	void writeAttribute(const QByteArray &str,const QByteArray &var);
	QByteArray readAttribute(const QByteArray &str);
	void addDataExportConfig(const QUuid &serviceId,const ISensorStorage::DataExportConfig &cfg);
	bool hasDataExportConfig(const QUuid &serviceId);
	ISensorStorage::DataExportConfig getDataExportConfig(const QUuid &serviceId);
	void removeDataExportConfig(const QUuid &serviceId);
	QList<QUuid> allDataExportServices();
	void storeDeviceName(const QByteArray &name);
	QSettings* settings();

	static ISensorStorage* preCreate(const QString &path,const QUuid &devId,
		const QByteArray &devName,SensorDef sensor,ISensorStorage::StoreMode mStoreMode,
		ISensorStorage::TimestampRule rule);
	//не создает саму базу, только создает папку и сохраняет mode и valueType
	static ISensorStorage* preOpen(const QString &path);

private:
	static ISensorStorage* makeStorage(const QString &path,
		const QUuid &devId,const QByteArray &devName,const SensorDef &sensor,
		ISensorStorage::StoreMode mode,ISensorStorage::TimestampRule rule);

private:
	QString mDbPath;
	QSettings *mSettings,*mExportSettings;
};

#endif // FSSENSORSTORAGEHELPER_H
