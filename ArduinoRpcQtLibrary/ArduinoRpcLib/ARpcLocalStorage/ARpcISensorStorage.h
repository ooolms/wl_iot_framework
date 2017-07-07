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
		BAD_MODE,
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
	static ARpcISensorStorage* preCreate(const QString &path,StoreMode mode,ARpcSensor::Type valType,
		TimestampRule rule);
		//не создает саму базу, только создает папку и сохраняет mode и valueType
	static ARpcISensorStorage* preOpen(const QString &path);
	ARpcSensor::Type sensorValuesType()const;
	QDir getDbDir()const;
	virtual bool open()=0;//use dbDir when opening
	virtual bool isOpened()const=0;
	void setDeviceName(const QString &name);
	QString getDeviceName();
	bool isDbDirSet()const;
	TimestampRule getTimestampRule()const;
	void close();

public:
	static QString storeModeToString(StoreMode mode);
	static StoreMode storeModeFromString(const QString &str);
	static QString timestampRuleToString(TimestampRule rule);
	static bool timestampRuleFromString(const QString &str,TimestampRule &rule);

public:
	virtual StoreMode getStoreMode()const=0;
	virtual bool writeSensorValue(const ARpcISensorValue *val)=0;
//	virtual bool TimestampRule getTimestampRule();
	virtual ARpcSensor::Type effectiveValuesType()const=0;
	virtual TimestampRule fixTimestampRule(TimestampRule rule)=0;
		//в некоторых режимах метки времени могут быть удалены или локальные метки времени могут
		//быть заменены на глобальные, тогда effectiveValuesType!=sensorValuesType

signals:
	void newValueWritten(const ARpcISensorValue *value);

protected:
	virtual void closeInternal()=0;
	static QString settingsFileRelPath();
	ARpcSensor::Type defaultEffectiveValuesType(TimestampRule rule);

private:
	static ARpcISensorStorage* makeStorage(ARpcSensor::Type valType,StoreMode mode);

protected:
	ARpcSensor::Type valueType;
	ARpcISensorStorage::TimestampRule timestampRule;
	QDir dbDir;
	bool dbDirSet;
};

#endif // ARPCISENSORSTORAGE_H
