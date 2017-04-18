#ifndef ARPCISENSORSTORAGE_H
#define ARPCISENSORSTORAGE_H

#include "ARpcBase/ARpcSensor.h"
#include "ARpcBase/ARpcISensorValue.h"
#include <QObject>
#include <QDir>
#include <QSettings>

//CRIT втащить сюда timestampRule и effectiveValueType

class ARpcISensorStorage
	:public QObject
{
	Q_OBJECT
public:
	enum StoreMode
	{
		CONTINUOUS,
		MANUAL_SESSIONS,
		AUTO_SESSIONS,
		LAST_N_VALUES
	};
	enum TimestampRule
	{
		DONT_TOUCH,
		ADD_GT,//also replace TL with GT
		DROP_TIME
	};

protected:
	explicit ARpcISensorStorage(ARpcSensor::Type valType,QObject *parent=0);

public:
	virtual ~ARpcISensorStorage(){}//override, call "close" in child classes
	static ARpcISensorStorage* preCreate(const QString &path,StoreMode mode,ARpcSensor::Type valType);
		//не создает саму базу, только создает папку и сохраняет mode и valueType
	static ARpcISensorStorage* preOpen(const QString &path);
	ARpcSensor::Type sensorValuesType()const;
	QDir getDbDir()const;
	virtual bool open()=0;//use dbDir when opening

public:
	virtual StoreMode getStoreMode()const=0;
	virtual bool writeSensorValue(const ARpcISensorValue *val)=0;
	virtual ARpcSensor::Type effectiveValuesType()const=0;
		//в некоторых режимах метки времени могут быть удалены или локальные метки времени могут
		//быть заменены на глобальные, тогда effectiveValuesType!=sensorValuesType
	void close();

protected:
	virtual void closeInternal()=0;
	static QString settingsFileRelPath();
	static QString timestampRuleToString(TimestampRule rule);
	static bool timestampRuleFromString(const QString &str,TimestampRule &rule);
	ARpcSensor::Type defaultEffectiveValuesType(TimestampRule rule);

private:
	static ARpcISensorStorage* makeStorage(ARpcSensor::Type valType,StoreMode mode);

protected:
	ARpcSensor::Type valueType;
	QDir dbDir;
};

#endif // ARPCISENSORSTORAGE_H
