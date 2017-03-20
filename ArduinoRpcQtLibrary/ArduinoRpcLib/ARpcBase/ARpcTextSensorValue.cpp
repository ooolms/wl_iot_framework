#include "ARpcTextSensorValue.h"

ARpcSensor::Type ARpcTextSensorValue::type()const
{
	return ARpcSensor::TEXT;
}

bool ARpcTextSensorValue::parse(const ARpcMessage &m)
{
	if(m.args.isEmpty())return false;
	text=m.args.join(" ");
	return true;
}

const QString &ARpcTextSensorValue::value() const
{
	return text;
}
