#ifndef VDILENGINEHELPER_H
#define VDILENGINEHELPER_H

#include "VDIL/core/IEngineHelper.h"
#include "../AccessManagement/AccessPolicyTypes.h"

class AccessMgr;

class EngineHelper
	:public WLIOTVDIL::IEngineHelper
{
public:
	explicit EngineHelper(IdType uid);
	virtual WLIOT::RealDevice* devById(const QUuid &id)override;
	virtual bool devStateById(const QUuid &id,WLIOT::DeviceState &state)override;
	virtual bool devIsConnected(const QUuid &id)override;
	virtual WLIOT::ISensorStorage *storageById(const WLIOT::StorageId &id)override;

private:
	IdType mUid;
	AccessMgr *acsMgr;
};

#endif // VDILENGINEHELPER_H
