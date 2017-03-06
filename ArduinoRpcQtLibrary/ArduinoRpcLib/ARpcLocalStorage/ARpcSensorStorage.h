#ifndef ARPCSENSORSTORAGE_H
#define ARPCSENSORSTORAGE_H

#include "ARpcBase/ARpcSensor.h"
#include <QObject>

class ARpcSensorStorage
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
		LAST_SESSION
	};

public:
	explicit ARpcSensorStorage(QObject *parent=0);
	bool create(const QString &path);
	bool open(const QString &path);
	bool isOpened()const;
	virtual void setStoreMode(StoreMode mode)=0;
	StoreMode getStoreMode()const;

protected:
	StoreMode storeMode;

private:
	bool opened;
};

#endif // ARPCSENSORSTORAGE_H
