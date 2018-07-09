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

#ifndef ARPCISENSORSTORAGE_H
#define ARPCISENSORSTORAGE_H

#include "ARpcBase/ARpcSensor.h"
#include "ARpcBase/ARpcISensorValue.h"
#include <QObject>
#include <QDir>
#include <QSettings>
#include <QUuid>

class ARpcISensorStorage
	:public QObject
{
	Q_OBJECT

public:
	enum StoreMode
	{
		BAD_MODE,
		CONTINUOUS,
		MANUAL_SESSIONS,
		AUTO_SESSIONS,
		LAST_N_VALUES,
		LAST_VALUE_IN_MEMORY
	};

	enum TimestampRule
	{
		DONT_TOUCH,
		ADD_GT,
		//also replace TL with GT
		DROP_TIME
	};

protected:
	explicit ARpcISensorStorage(ARpcSensor sensor,const QUuid &devId,const QByteArray &devName,QObject *parent=0);

public:
	virtual ~ARpcISensorStorage()
	{
	}//override, call "close" in child classes

	static ARpcISensorStorage* preCreate(const QString &path,StoreMode mode,ARpcSensor sensor,const QUuid &devId,
		const QByteArray &devName,TimestampRule rule);
	//не создает саму базу, только создает папку и сохраняет mode и valueType
	static ARpcISensorStorage* preOpen(const QString &path);
	const ARpcSensor& sensor() const;
	QDir getDbDir() const;
	QUuid deviceId();
	QByteArray deviceName();
	void setDeviceName(const QByteArray &name);
	bool isDbDirSet() const;
	TimestampRule getTimestampRule() const;
	ARpcSensor::Type effectiveValuesType()const;
	void close();
	void writeAttribute(const QByteArray &str,const QVariant &var);
	QVariant readAttribute(const QByteArray &str);

public:
	virtual bool open()=0;//use dbDir when opening
	virtual bool isOpened() const=0;
	virtual quint64 valuesCount()=0;
	virtual ARpcISensorValue* valueAt(quint64 index)=0;
	virtual StoreMode getStoreMode() const=0;
	virtual bool writeSensorValue(const ARpcISensorValue *val)=0;
	virtual TimestampRule fixTimestampRule(TimestampRule rule)=0;
	//в некоторых режимах метки времени могут быть удалены или локальные метки времени могут
	//быть заменены на глобальные, тогда effectiveValuesType!=sensorValuesType

public:
	static QByteArray storeModeToString(StoreMode mode);
	static StoreMode storeModeFromString(const QByteArray &str);
	static QByteArray timestampRuleToString(TimestampRule rule);
	static bool timestampRuleFromString(const QByteArray &str,TimestampRule &rule);

signals:
	void newValueWritten(const ARpcISensorValue *value);

protected:
	virtual void closeInternal()=0;
	static QString settingsFileRelPath();
	ARpcSensor::Type defaultEffectiveValuesType(TimestampRule rule);

private:
	static ARpcISensorStorage* makeStorage(const ARpcSensor &sensor,const QUuid &devId,const QByteArray &devName,
		StoreMode mode);

protected:
	QUuid mDeviceId;
	QByteArray mDeviceName;
	ARpcSensor mSensor;
	ARpcISensorStorage::TimestampRule timestampRule;
	ARpcSensor::Type effectiveValType;
	QDir dbDir;
	bool dbDirSet;
};

#endif // ARPCISENSORSTORAGE_H
