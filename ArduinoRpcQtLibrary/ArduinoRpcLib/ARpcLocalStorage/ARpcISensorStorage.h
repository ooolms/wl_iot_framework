#ifndef ARPCISENSORSTORAGE_H
#define ARPCISENSORSTORAGE_H

#include "ARpcBase/ARpcSensor.h"
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
	static ARpcISensorStorage* create(const QString &path,StoreMode mode);
	static ARpcISensorStorage* open(const QString &path);
	virtual StoreMode getStoreMode()const=0;

protected:
	virtual bool createInternal(const QString &path)=0;
	virtual bool openInternal(const QString &path)=0;

private:
	static ARpcISensorStorage* makeStorage(StoreMode mode);
};

#endif // ARPCISENSORSTORAGE_H