#ifndef ARPCSINGLESENSORVALUE_H
#define ARPCSINGLESENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"
#include <QVector>

class ARpcSingleSensorValue
	:public ARpcISensorValue
{
public:
	explicit ARpcSingleSensorValue(int dims);//no timestamp
	explicit ARpcSingleSensorValue(int dims,bool localTimeStamp);//true - local, false - global
	virtual ARpcSensor::Type type()const override;
	virtual bool parse(const ARpcMessage &m)override;
	const QVector<double>& values()const;

private:
	ARpcSensor::Type valueType;
	int dimensions;
	qint64 timestamp;
	QVector<double> valuesList;
};

#endif // ARPCSINGLESENSORVALUE_H
