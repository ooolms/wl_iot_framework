#ifndef GDILENGINEHELPER_H
#define GDILENGINEHELPER_H

#include "GDIL/core/IEngineHelper.h"
#include "../AccessManagement/AccessPolicyTypes.h"

class AccessMgr;

class GDILEngineHelper
	:public WLIOTGDIL::IEngineHelper
{
public:
	explicit GDILEngineHelper(IdType uid);
	virtual bool devStateById(const QUuid &id,WLIOT::DeviceState &state)override;
	virtual bool devIsConnected(const QUuid &id)override;
	virtual WLIOT::ISensorStorage *storageById(const WLIOT::StorageId &id)override;

private:
	IdType mUid;
	AccessMgr *acsMgr;
};

#endif // GDILENGINEHELPER_H
