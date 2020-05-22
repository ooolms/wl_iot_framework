#include "GDILEngineHelper.h"
#include "../ServerInstance.h"
#include "../MainServerConfig.h"
#include "../AccessManagement/AccessMgr.h"

using namespace WLIOT;

GDILEngineHelper::GDILEngineHelper(IdType uid)
{
	mUid=uid;
	acsMgr=&MainServerConfig::accessManager;
}

bool GDILEngineHelper::devStateById(const QUuid &id,DeviceState &state)
{
	if(!acsMgr->userCanAccessDevice(id,mUid,DevicePolicyActionFlag::READ_STATE))
		return false;
	RealDevice *dev=ServerInstance::inst().devices()->deviceById(id);
	if(!dev)return false;
	return dev->getState(state);
}

bool GDILEngineHelper::devIsConnected(const QUuid &id)
{
	if(!acsMgr->userCanAccessDevice(id,mUid,DevicePolicyActionFlag::READ_STATE))
		return false;
	RealDevice *dev=ServerInstance::inst().devices()->deviceById(id);
	if(!dev)return false;
	return dev->isConnected();
}

ISensorStorage* GDILEngineHelper::storageById(const StorageId &id)
{
	if(!acsMgr->userCanAccessDevice(id.deviceId,mUid,DevicePolicyActionFlag::READ_STORAGES))
		return 0;
	return ServerInstance::inst().storages()->existingStorage(id);
}
