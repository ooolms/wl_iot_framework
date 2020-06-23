#include "wliot/devices/IHighLevelDeviceBackend.h"
#include "wliot/devices/RealDevice.h"

using namespace WLIOT;

IHighLevelDeviceBackend::IHighLevelDeviceBackend(QObject *parent)
	:QObject(parent)
{
}

IHighLevelDeviceBackend::~IHighLevelDeviceBackend()
{
}
