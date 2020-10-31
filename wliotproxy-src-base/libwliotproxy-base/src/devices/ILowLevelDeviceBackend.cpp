#include "wliot/devices/ILowLevelDeviceBackend.h"

using namespace WLIOT;

ILowLevelDeviceBackend::ILowLevelDeviceBackend(const QString &hwAddress,QObject *parent)
	:QObject(parent)
{
	mHwAddress=hwAddress;
}

QString ILowLevelDeviceBackend::hwAddress()const
{
	return mHwAddress;
}
