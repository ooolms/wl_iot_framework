#include "ARpcISensorValue.h"
#include "ARpcPacketSensorValue.h"
#include "ARpcSingleSensorValue.h"
#include "ARpcTextSensorValue.h"

ARpcISensorValue::ARpcISensorValue()
{
	timestamp=0;
}

qint64 ARpcISensorValue::time()const
{
	return timestamp;
}

void ARpcISensorValue::setTime(qint64 t)
{
	timestamp=t;
}
