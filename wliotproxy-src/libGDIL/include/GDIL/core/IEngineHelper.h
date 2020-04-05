#ifndef IENGINEHELPER_H
#define IENGINEHELPER_H

#include "wliot/devices/RealDevice.h"
#include "wliot/storages/ISensorStorage.h"
#include "wliot/storages/StorageId.h"

class IEngineHelper
{
public:
	virtual ~IEngineHelper(){}
	virtual RealDevice* devById(const QUuid &id)=0;
	virtual ISensorStorage* storageById(const StorageId &id)=0;
};

#endif // IENGINEHELPER_H
