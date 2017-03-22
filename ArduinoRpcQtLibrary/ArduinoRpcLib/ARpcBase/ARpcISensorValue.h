#ifndef ARPCISENSORVALUE_H
#define ARPCISENSORVALUE_H

#include "ARpcBase/ARpcSensor.h"
#include "ARpcBase/ARpcMessage.h"

class ARpcISensorValue
{
public:
	virtual ARpcSensor::Type type()const=0;
	virtual ~ARpcISensorValue(){}
	virtual bool parse(ARpcMessage m)=0;
};

#endif // ARPCISENSORVALUE_H
