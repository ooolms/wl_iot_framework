#include "VDILEngineHelper.h"
#include "../ServerInstance.h"
#include "../MainServerConfig.h"
#include "../AccessManagement/AccessMgr.h"

using namespace WLIOT;

VDILEngineHelper::VDILEngineHelper(IdType uid)
{
	mUid=uid;
	acsMgr=&MainServerConfig::accessManager;
}

bool VDILEngineHelper::devStateById(const QUuid &id,DeviceState &state)
{
	if(!acsMgr->userCanAccessDevice(id,mUid,DevicePolicyActionFlag::READ_STATE))
		return false;
	RealDevice *dev=ServerInstance::inst().devices()->deviceById(id);
	if(!dev)return false;
	return dev->getState(state);
}

bool VDILEngineHelper::devIsConnected(const QUuid &id)
{
	if(!acsMgr->userCanAccessDevice(id,mUid,DevicePolicyActionFlag::READ_STATE))
		return false;
	RealDevice *dev=ServerInstance::inst().devices()->deviceById(id);
	if(!dev)return false;
	return dev->isConnected();
}

ISensorStorage* VDILEngineHelper::storageById(const StorageId &id)
{
	if(!acsMgr->userCanAccessDevice(id.deviceId,mUid,DevicePolicyActionFlag::READ_STORAGES))
		return 0;
	return ServerInstance::inst().storages()->existingStorage(id);
}


RealDevice* VDILEngineHelper::devById(const QUuid &id)
{
	return ServerInstance::inst().devices()->deviceById(id);
}
