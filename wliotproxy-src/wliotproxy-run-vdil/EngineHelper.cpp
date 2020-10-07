#include "EngineHelper.h"

using namespace WLIOT;
using namespace WLIOTClient;

EngineHelper::EngineHelper(ServerInstance *s)
{
	srv=s;
}

bool EngineHelper::devStateById(const QUuid &id,DeviceState &state)
{
	RealDevice *dev=srv->devices()->devById(id);
	if(!dev)return false;
	return dev->getState(state);
}

bool EngineHelper::devIsConnected(const QUuid &id)
{
	RealDevice *dev=srv->devices()->devById(id);
	if(!dev)return false;
	return dev->isConnected();
}

ISensorStorage* EngineHelper::storageById(const StorageId &id)
{
	ISensorStorage *s=srv->storages()->existingStorage(id);
	if(s)s->open();
	return s;
}

RealDevice* EngineHelper::devById(const QUuid &id)
{
	return srv->devices()->devById(id);
}
