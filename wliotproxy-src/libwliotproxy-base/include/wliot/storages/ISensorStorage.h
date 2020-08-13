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

#ifndef ISENSORSTORAGE_H
#define ISENSORSTORAGE_H

#include <QObject>
#include "wliot/devices/SensorDef.h"
#include "wliot/devices/SensorValue.h"
#include "wliot/storages/VeryBigArray.h"
#include <QUuid>

namespace WLIOT
{
	class ISensorStorage
		:public QObject
	{
		Q_OBJECT
	public:
		enum StoreMode
		{
			INVALID_MODE,
			CONTINUOUS,
			MANUAL_SESSIONS,
			AUTO_SESSIONS,
			LAST_N_VALUES,
			LAST_N_VALUES_IN_MEMORY
		};

		enum TimestampRule
		{
			INVALID_RULE,
			DONT_TOUCH,
			ADD_GT,
			DROP_TIME
		};

		typedef QMap<QByteArray,QByteArray> DataExportConfig;

	public:
		explicit ISensorStorage(const QUuid &devId,const QByteArray &devName,SensorDef sensor,StoreMode stMode,
			ISensorStorage::TimestampRule tsRule,SensorDef::Type storedValType,QObject *parent=nullptr);
		virtual ~ISensorStorage()
		{
		}//override, call "close" in child classes
		const SensorDef& sensor()const;
		QUuid deviceId();
		QByteArray deviceName();
		TimestampRule timestampRule()const;
		SensorDef::Type storedValuesType()const;
		StoreMode storeMode()const;

	public:
		virtual bool open()=0;
		virtual void close()=0;
		virtual bool isOpened()const=0;
		virtual quint64 valuesCount()=0;
		virtual SensorValue* valueAt(quint64 index)=0;
		virtual SensorValue* lastValue()=0;//для оптимизации
		virtual bool writeSensorValue(const SensorValue *val)=0;
		virtual void writeAttribute(const QByteArray &str,const QByteArray &var)=0;
		virtual QByteArray readAttribute(const QByteArray &str)=0;
		virtual void addDataExportConfig(const QUuid &serviceId,const DataExportConfig &cfg)=0;
		virtual bool hasDataExportConfig(const QUuid &serviceId)=0;
		virtual DataExportConfig getDataExportConfig(const QUuid &serviceId)=0;
		virtual void removeDataExportConfig(const QUuid &serviceId)=0;
		virtual QList<QUuid> allDataExportServices()=0;
		virtual bool values(quint64 index,quint64 count,quint64 step,VeryBigArray<SensorValue*> &vals);
		virtual bool hasGTIndex();
		virtual quint64 findInGTIndex(qint64 ts);

	public:
		static QByteArray storeModeToString(StoreMode mStoreMode);
		static StoreMode storeModeFromString(const QByteArray &str);
		static QByteArray timestampRuleToString(TimestampRule rule);
		static TimestampRule timestampRuleFromString(const QByteArray &str);

	signals:
		void newValueWritten(const WLIOT::SensorValue *value);

	protected:
		static SensorDef::Type defaultStoredValuesType(SensorDef::Type sType,TimestampRule rule);

	protected:
		QUuid mDeviceId;
		QByteArray mDeviceName;
		SensorDef mSensor;
		ISensorStorage::TimestampRule mTimestampRule;
		SensorDef::Type mStoredValuesType;
		StoreMode mStoreMode;
	};
}

#endif // ISENSORSTORAGE_H
