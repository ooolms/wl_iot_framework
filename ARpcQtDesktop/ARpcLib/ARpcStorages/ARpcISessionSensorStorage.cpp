#include "ARpcISessionSensorStorage.h"

ARpcISessionSensorStorage::ARpcISessionSensorStorage(
	const QUuid &devId,const QByteArray &devName,ARpcSensorDef sensor,ARpcISensorStorage::StoreMode stMode,
	ARpcISensorStorage::TimestampRule tsRule,ARpcSensorDef::Type storedValType,QObject *parent)
	:ARpcISensorStorage(devId,devName,sensor,stMode,tsRule,storedValType,parent)
{
}

bool ARpcISessionSensorStorage::values(const QUuid &sessionId,quint64 index,quint64 count,quint64 step,
	VeryBigArray<ARpcSensorValue*> &vals)
{
	if(!isOpened())
		return false;
	vals.clear();
	for(quint64 i=0;i<count;++i)
	{
		ARpcSensorValue *v=valueAt(sessionId,index+i*step);
		if(v)vals.append(v);
		else break;
	}
	return true;
}
