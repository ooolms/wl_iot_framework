#include "wliot/devices/IHighLevelDeviceBackend.h"
#include "wliot/devices/RealDevice.h"

using namespace WLIOT;

IHighLevelDeviceBackend::IHighLevelDeviceBackend(QObject *parent)
	:QObject(parent)
{
	mDev=0;
}

IHighLevelDeviceBackend::~IHighLevelDeviceBackend()
{
}

void IHighLevelDeviceBackend::setDevice(RealDevice *dev)
{
	mDev=dev;
}

RealDevice* IHighLevelDeviceBackend::dev()
{
	return mDev;
}
