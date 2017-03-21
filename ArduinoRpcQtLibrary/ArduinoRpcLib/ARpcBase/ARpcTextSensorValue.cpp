#include "ARpcTextSensorValue.h"

ARpcSensor::Type ARpcTextSensorValue::type()const
{
	return ARpcSensor::TEXT;
}

bool ARpcTextSensorValue::parse(ARpcMessage m)
{
	if(m.args.count()<2)return false;
	m.args.removeAt(0);
	text=m.args.join(" ");
	return true;
}

const QString &ARpcTextSensorValue::value() const
{
	return text;
}
