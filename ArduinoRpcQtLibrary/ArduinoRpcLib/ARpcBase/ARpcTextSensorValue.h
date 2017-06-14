#ifndef ARPCTEXTSENSORVALUE_H
#define ARPCTEXTSENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"

class ARpcTextSensorValue
	:public ARpcISensorValue
{
public:
	ARpcTextSensorValue();
	virtual ARpcSensor::Type type()const override;
	virtual bool parse(ARpcMessage m)override;
	const QString& value()const;
	void fromData(const char *str,int size);

private:
	QString text;
};

#endif // ARPCTEXTSENSORVALUE_H
