#ifndef GDILENGINEHELPER_H
#define GDILENGINEHELPER_H

#include "GDIL/core/IEngineHelper.h"

class GDILEngineHelper
	:public IEngineHelper
{
public:
	virtual RealDevice *devById(const QUuid &id) override;
	virtual ISensorStorage *storageById(const StorageId &id) override;
};

#endif // GDILENGINEHELPER_H
