#include "ARpcTextSensorValue.h"
#include <QDateTime>

ARpcTextSensorValue::ARpcTextSensorValue()
{
	timestamp=0;
}

ARpcSensor::Type ARpcTextSensorValue::type()const
{
	return ARpcSensor::TEXT;
}

bool ARpcTextSensorValue::parse(ARpcMessage m)
{
	if(m.args.count()<2)return false;
	m.args.removeAt(0);
	timestamp=QDateTime::currentMSecsSinceEpoch();
	text=m.args.join(" ");
	return true;
}

const QString &ARpcTextSensorValue::value()const
{
	return text;
}

void ARpcTextSensorValue::fromData(const char *str,int size)
{
	text=QString::fromUtf8(str,size);
}
