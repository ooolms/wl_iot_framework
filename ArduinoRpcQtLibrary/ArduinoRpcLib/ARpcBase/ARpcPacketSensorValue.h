#ifndef ARPCPACKETSENSORVALUE_H
#define ARPCPACKETSENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"
#include <QVector>

class ARpcPacketSensorValue
	:public ARpcISensorValue
{
public:
	explicit ARpcPacketSensorValue(int dims);//no timestamp
	explicit ARpcPacketSensorValue(int dims,bool localTimeStamp);//true - local, false - global
	virtual ARpcSensor::Type type()const override;
	virtual bool parse(const ARpcMessage &m)override;
	const QVector<QVector<double>>& values()const;

private:
	ARpcSensor::Type valueType;
	int dimensions;
	qint64 timestamp;
	QVector<QVector<double>> valuesList;
};

#endif // ARPCPACKETSENSORVALUE_H
