#ifndef ARPCPACKETSENSORVALUE_H
#define ARPCPACKETSENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"
#include <QVector>

class ARpcPacketSensorValue
	:public ARpcISensorValue
{
public:
	explicit ARpcPacketSensorValue(quint32 dims);//no timestamp
	explicit ARpcPacketSensorValue(quint32 dims,bool localTimeStamp);//true - local, false - global
	virtual ARpcSensor::Type type()const override;
	virtual bool parse(ARpcMessage m)override;
	const QVector<float>& values()const;
	float at(quint32 valIndex,quint32 dimension)const;
	qint64 time()const;
	void setTime(qint64 t);
	quint32 dims()const;//dimensions
	quint32 valuesCount()const;
	void fromData(const float *vals,quint32 dims,quint32 count);

private:
	ARpcSensor::Type valueType;
	quint32 dimensions;
	quint32 valCount;
	qint64 timestamp;
	QVector<float> valuesList;//{valCount} строк последовательно по {dimensions} записей
};

#endif // ARPCPACKETSENSORVALUE_H
