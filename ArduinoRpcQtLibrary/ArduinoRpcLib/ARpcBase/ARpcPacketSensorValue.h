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
	virtual bool parse(ARpcMessage m)override;
	const float* values()const;
	float at(int valIndex,int dimension)const;
	qint64 time()const;
	int dims()const;//dimensions
	int valuesCount()const;

private:
	ARpcSensor::Type valueType;
	int dimensions;
	int valCount;
	qint64 timestamp;
	float *valuesList;//{valCount} строк последовательно по {dimensions} записей
};

#endif // ARPCPACKETSENSORVALUE_H
