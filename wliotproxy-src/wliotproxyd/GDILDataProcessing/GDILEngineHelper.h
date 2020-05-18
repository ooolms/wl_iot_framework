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
	virtual bool devStateById(const QUuid &id,DeviceState &state)override;
	virtual bool devIsConnected(const QUuid &id)override;
	virtual ISensorStorage *storageById(const StorageId &id)override;

private:
	IdType mUid;
	AccessMgr *acsMgr;
};

#endif // GDILENGINEHELPER_H
