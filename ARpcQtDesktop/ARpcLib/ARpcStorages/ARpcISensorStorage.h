#ifndef ARPCISENSORSTORAGE_H
#define ARPCISENSORSTORAGE_H

#include <QObject>
#include "ARpcBase/ARpcSensorDef.h"
#include "ARpcBase/ARpcSensorValue.h"
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
		LAST_N_VALUES_IN_MEMORY
	};

	enum TimestampRule
	{
		DONT_TOUCH,
		ADD_GT,
		//also replace TL with GT
		DROP_TIME
	};

	typedef QMap<QByteArray,QByteArray> DataExportConfig;

public:
	explicit ARpcISensorStorage(const QUuid &devId,const QByteArray &devName,ARpcSensorDef sensor,StoreMode stMode,
		ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedValType,QObject *parent=nullptr);
	virtual ~ARpcISensorStorage()
	{
	}//override, call "close" in child classes
	const ARpcSensorDef& sensor()const;
	QUuid deviceId();
	QByteArray deviceName();
	TimestampRule timestampRule()const;
	ARpcSensorDef::Type storedValuesType()const;
	StoreMode storeMode()const;

public:
	virtual bool open()=0;
	virtual void close()=0;
	virtual bool isOpened()const=0;
	virtual quint64 valuesCount()=0;
	virtual ARpcSensorValue* valueAt(quint64 index)=0;
	virtual bool writeSensorValue(const ARpcSensorValue *val)=0;
	virtual void writeAttribute(const QByteArray &str,const QVariant &var)=0;
	virtual QVariant readAttribute(const QByteArray &str)=0;
	virtual void addDataExportConfig(const QByteArray &serviceType,const DataExportConfig &cfg)=0;
	virtual bool hasDataExportConfig(const QByteArray &serviceType)=0;
	virtual DataExportConfig getDataExportConfig(const QByteArray &serviceType)=0;
	virtual void removeDataExportConfig(const QByteArray &serviceType)=0;
	virtual QByteArrayList allDataExportServices()=0;

public:
	static QByteArray storeModeToString(StoreMode mStoreMode);
	static StoreMode storeModeFromString(const QByteArray &str);
	static QByteArray timestampRuleToString(TimestampRule rule);
	static bool timestampRuleFromString(const QByteArray &str,TimestampRule &rule);

signals:
	void newValueWritten(const ARpcSensorValue *value);

protected:
	QUuid mDeviceId;
	QByteArray mDeviceName;
	ARpcSensorDef mSensor;
	ARpcISensorStorage::TimestampRule mTimestampRule;
	ARpcSensorDef::Type mStoredValuesType;
	StoreMode mStoreMode;
};

#endif // ARPCISENSORSTORAGE_H
