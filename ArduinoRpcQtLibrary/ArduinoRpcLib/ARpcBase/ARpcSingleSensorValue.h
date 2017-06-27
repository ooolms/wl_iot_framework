#ifndef ARPCSINGLESENSORVALUE_H
#define ARPCSINGLESENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"
#include <QVector>

class ARpcSingleSensorValue
	:public ARpcISensorValue
{
public:
	typedef double ValueType;

public:
	explicit ARpcSingleSensorValue(quint32 dims);//no timestamp
	explicit ARpcSingleSensorValue(quint32 dims,bool localTimeStamp);//true - local, false - global
	virtual ARpcSensor::Type type()const override;
	virtual bool parse(ARpcMessage m)override;
	const QVector<ValueType>& values()const;
	quint32 dims()const;//dimensions
	void fromData(const QVector<ValueType> &vals);
	void fromData(const ValueType *vals,quint32 dims);

private:
	ARpcSensor::Type valueType;
	quint32 dimensions;
	QVector<ValueType> valuesList;
};

#endif // ARPCSINGLESENSORVALUE_H
