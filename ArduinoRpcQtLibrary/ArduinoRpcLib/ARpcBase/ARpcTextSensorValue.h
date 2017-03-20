#ifndef ARPCTEXTSENSORVALUE_H
#define ARPCTEXTSENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"

class ARpcTextSensorValue
	:public ARpcISensorValue
{
public:
	virtual ARpcSensor::Type type()const;
	virtual bool parse(const ARpcMessage &m);
	virtual const QString& value()const;

private:
	QString text;
};

#endif // ARPCTEXTSENSORVALUE_H
