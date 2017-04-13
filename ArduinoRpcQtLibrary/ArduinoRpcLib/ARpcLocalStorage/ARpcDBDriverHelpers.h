#ifndef ARPCDBDRIVERHELPERS_H
#define ARPCDBDRIVERHELPERS_H

#include "ARpcDBDriverFixedBlocks.h"
#include "ARpcDBDriverChainedBlocks.h"
#include "ARpcBase/ARpcISensorValue.h"
#include "ARpcISensorStorage.h"

class ARpcDBDriverHelpers
{
public:
	explicit ARpcDBDriverHelpers();
	explicit ARpcDBDriverHelpers(ARpcISensorStorage::TimestampRule rule);
	QByteArray packSensorValue(const ARpcISensorValue *val);
	ARpcISensorValue* unpackSensorValue(ARpcSensor::Type type,const QByteArray &data);
	void detectIfHasTime(ARpcSensor::Type type,int &hasTime);
	void getTimeFromVal(const ARpcISensorValue *val,int &hasTime,qint64 &timestamp);
	QVector<quint32> sizesForFixedBlocksDb(const ARpcISensorValue &templateValue);

private:
	ARpcISensorStorage::TimestampRule timeRule;
};

#endif // ARPCDBDRIVERHELPERS_H
