#ifndef DATACOLLECTIONUNIT_H
#define DATACOLLECTIONUNIT_H

#include "ARpcBase/ARpcDevice.h"
#include "ARpcLocalStorage/ARpcISensorStorage.h"
#include "ARpcBase/ARpcSimpleMsgDispatch.h"
#include <QObject>

class ARpcContinuousStorage;
class ARpcSessionStorage;
class ARpcLastNValuesStorage;

class DataCollectionUnit
	:public QObject
{
	Q_OBJECT
public:
	explicit DataCollectionUnit(ARpcDevice *dev,ARpcISensorStorage *stor,
		const ARpcSensor &sensorDescr,QObject *parent=0);
	virtual ~DataCollectionUnit();

signals:
	void errorMessage(const QString &msg);
	void infoMessage(const QString &msg);

private slots:
	void onRawMsg(const ARpcMessage &m);

private:
	void processMeasurementMsg(const ARpcMessage &m);

private:
	union
	{
		ARpcContinuousStorage *contStor;
		ARpcSessionStorage *sessStor;
		ARpcLastNValuesStorage *lastNStor;
	}stors;
	ARpcISensorStorage::StoreMode storeMode;
	ARpcDevice *device;
	ARpcSensor sensorDescriptor;
};

#endif // DATACOLLECTIONUNIT_H
