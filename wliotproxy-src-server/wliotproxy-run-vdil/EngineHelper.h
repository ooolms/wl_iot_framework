#ifndef ENGINEHELPER_H
#define ENGINEHELPER_H

#include "VDIL/core/IEngineHelper.h"
#include <wliot/client/ServerInstance.h>

class AccessMgr;

class EngineHelper
	:public WLIOTVDIL::IEngineHelper
{
public:
	explicit EngineHelper(WLIOTClient::ServerInstance *s);
	virtual WLIOT::RealDevice* devById(const QUuid &id)override;
	virtual bool devStateById(const QUuid &id,WLIOT::DeviceState &state)override;
	virtual bool devIsConnected(const QUuid &id)override;
	virtual WLIOT::ISensorStorage *storageById(const WLIOT::StorageId &id)override;

private:
	WLIOTClient::ServerInstance *srv;
};

#endif // ENGINEHELPER_H
