#ifndef ARPCISENSORVALUE_H
#define ARPCISENSORVALUE_H

#include "ARpcBase/ARpcSensor.h"
#include "ARpcBase/ARpcMessage.h"

class ARpcISensorValue
{
public:
	ARpcISensorValue();
	virtual ARpcSensor::Type type()const=0;
	virtual ~ARpcISensorValue(){}
	virtual bool parse(ARpcMessage m)=0;
	qint64 time()const;
	void setTime(qint64 t);

protected:
	qint64 timestamp;
};

#endif // ARPCISENSORVALUE_H
