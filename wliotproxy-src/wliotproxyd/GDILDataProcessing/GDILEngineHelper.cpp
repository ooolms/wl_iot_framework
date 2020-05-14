#include "GDILEngineHelper.h"
#include "../ServerInstance.h"
#include "../MainServerConfig.h"
#include "../AccessManagement/AccessMgr.h"

GDILEngineHelper::GDILEngineHelper(IdType uid)
{
	mUid=uid;
	acsMgr=&MainServerConfig::accessManager;
}

RealDevice* GDILEngineHelper::devById(const QUuid &id)
{
	if(!acsMgr->userCanAccessDevice(id,mUid,DevicePolicyActionFlag::EXECUTE_COMMANDS))
		return 0;
	return ServerInstance::inst().devices()->deviceById(id);
}

ISensorStorage* GDILEngineHelper::storageById(const StorageId &id)
{
	if(!acsMgr->userCanAccessDevice(id.deviceId,mUid,DevicePolicyActionFlag::READ_STORAGES))
		return 0;
	return ServerInstance::inst().storages()->existingStorage(id);
}

QString GDILEngineHelper::findDevName(const QUuid &id)
{
	return QString::fromUtf8(ServerInstance::inst().devNames()->deviceName(id));
}
