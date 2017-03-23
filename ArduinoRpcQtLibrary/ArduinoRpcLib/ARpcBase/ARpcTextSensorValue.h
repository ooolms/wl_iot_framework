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
	qint64 time()const;
	void setTime(qint64 t);

private:
	qint64 timestamp;
	QString text;
};

#endif // ARPCTEXTSENSORVALUE_H
