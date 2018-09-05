#include "ARpcStorageId.h"

static const int metaTypeIdForARpcStorageId=qMetaTypeId<ARpcStorageId>();

uint qHash(const ARpcStorageId &id,uint seed)
{
	return qHash(id.deviceId,seed)&qHash(id.sensorName,seed);
}
