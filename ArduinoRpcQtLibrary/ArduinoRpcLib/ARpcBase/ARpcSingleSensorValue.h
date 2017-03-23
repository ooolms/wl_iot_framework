#ifndef ARPCSINGLESENSORVALUE_H
#define ARPCSINGLESENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"
#include <QVector>

class ARpcSingleSensorValue
	:public ARpcISensorValue
{
public:
	explicit ARpcSingleSensorValue(quint32 dims);//no timestamp
	explicit ARpcSingleSensorValue(quint32 dims,bool localTimeStamp);//true - local, false - global
	virtual ARpcSensor::Type type()const override;
	virtual bool parse(ARpcMessage m)override;
	const QVector<double>& values()const;
	qint64 time()const;
	void setTime(qint64 t);
	quint32 dims()const;//dimensions
	void fromData(const QVector<double> &vals);
	void fromData(const double *vals,quint32 dims);

private:
	ARpcSensor::Type valueType;
	quint32 dimensions;
	qint64 timestamp;
	QVector<double> valuesList;
};

#endif // ARPCSINGLESENSORVALUE_H
