#include "DeviceTypesPriority.h"
#include "wliot/devices/SerialDeviceBackend.h"
#include "wliot/devices/TcpDeviceBackend.h"
#include "wliot/devices/TcpSslDeviceBackend.h"
#include "wliot/devices/VirtualDeviceBackend.h"
#include "wliot/devices/HubDeviceBackend.h"

using namespace WLIOT;

DeviceTypesPriority::DeviceTypesPriority()
{
	int p=0;
	typePriorities[VirtualDeviceBackend::mBackendType]=++p;
	typePriorities[HubDeviceBackend::mBackendType]=++p;
	typePriorities[TcpDeviceBackend::mBackendType]=++p;
	typePriorities[TcpSslDeviceBackend::mBackendType]=++p;
	typePriorities[SerialDeviceBackend::mBackendType]=++p;
}

DeviceTypesPriority& DeviceTypesPriority::inst()
{
	static DeviceTypesPriority var;
	return var;
}

bool DeviceTypesPriority::shouldReplace(const QByteArray &existingBackendType,const QByteArray &replacingBackendType)
{
	return typePriorities.value(replacingBackendType,0)>typePriorities.value(existingBackendType,0);
}
