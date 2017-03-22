#ifndef ARPCISENSORSTORAGE_H
#define ARPCISENSORSTORAGE_H

#include "ARpcBase/ARpcSensor.h"
#include "ARpcBase/ARpcISensorValue.h"
#include <QObject>
#include <QDir>
#include <QSettings>

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
		LAST_VALUE,
		LAST_PACKET
	};

public:
	explicit ARpcISensorStorage(QObject *parent=0):QObject(parent){}
	virtual ~ARpcISensorStorage(){close();}
	static ARpcISensorStorage* preCreate(const QString &path,StoreMode mode,ARpcSensor::Type valType);
		//не создает саму базу, только создает папку и сохраняет mode и valType
	static ARpcISensorStorage* open(const QString &path);
	ARpcSensor::Type sensorValuesType()const;
	QDir getDbDir()const;

public:
	virtual StoreMode getStoreMode()const=0;
	virtual bool writeSensorValue(const ARpcISensorValue *val)=0;
	void close();

protected:
	virtual bool openInternal()=0;//use dbDir when opening
	virtual void closeInternal()=0;
	static QString settingsFileRelPath();

private:
	static ARpcISensorStorage* makeStorage(StoreMode mode);

protected:
	ARpcSensor::Type valueType;
	QDir dbDir;
};

#endif // ARPCISENSORSTORAGE_H
