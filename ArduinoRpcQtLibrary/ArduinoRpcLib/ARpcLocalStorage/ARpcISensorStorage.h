#ifndef ARPCISENSORSTORAGE_H
#define ARPCISENSORSTORAGE_H

#include "ARpcBase/ARpcSensor.h"
#include "ARpcBase/ARpcISensorValue.h"
#include <QObject>

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
	static ARpcISensorStorage* preCreate(const QString &path,StoreMode mode);
		//не создает саму базу, только создает папку и сохраняет StoreMode
	static ARpcISensorStorage* open(const QString &path);
	virtual StoreMode getStoreMode()const=0;
	virtual bool writeSensorValue(const ARpcISensorValue *val)=0;

protected:
	virtual bool openInternal(const QString &path)=0;

private:
	static ARpcISensorStorage* makeStorage(StoreMode mode);
};

#endif // ARPCISENSORSTORAGE_H
