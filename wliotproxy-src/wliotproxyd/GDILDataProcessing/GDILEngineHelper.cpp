#include "GDILEngineHelper.h"
#include "../ServerInstance.h"

RealDevice* GDILEngineHelper::devById(const QUuid &id)
{
	return ServerInstance::inst().devices()->deviceById(id);
}

ISensorStorage* GDILEngineHelper::storageById(const StorageId &id)
{
	return ServerInstance::inst().storages()->existingStorage(id);
}

QString GDILEngineHelper::findDevName(const QUuid &id)
{
	return QString::fromUtf8(ServerInstance::inst().devNames()->deviceName(id));
}
