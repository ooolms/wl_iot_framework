#ifndef GDILENGINEHELPER_H
#define GDILENGINEHELPER_H

#include "GDIL/core/IEngineHelper.h"
#include "../AccessManagement/AccessPolicyTypes.h"

class AccessMgr;

class GDILEngineHelper
	:public IEngineHelper
{
public:
	explicit GDILEngineHelper(IdType uid);
	virtual RealDevice *devById(const QUuid &id)override;
	virtual ISensorStorage *storageById(const StorageId &id)override;
	virtual QString findDevName(const QUuid &id)override;

private:
	IdType mUid;
	AccessMgr *acsMgr;
};

#endif // GDILENGINEHELPER_H
