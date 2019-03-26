#include "IotServerVirtualDeviceCallback.h"
#include "IotServerVirtualDeviceClient.h"

IotServerVirtualDeviceCallback::IotServerVirtualDeviceCallback(IotServerVirtualDeviceClient *dev)
	:QObject(dev)
{
	mDev=dev;
	dev->setDevEventsCallback(this);
}

void IotServerVirtualDeviceCallback::commandParamStateChanged(
	const QByteArray &cmd,int paramIndex,const QByteArray &value)
{
	mDev->commandParamStateChanged(cmd,paramIndex,value);
}

void IotServerVirtualDeviceCallback::additionalStateChanged(
	const QByteArray &paramName,const QByteArray &value)
{
	mDev->additionalStateChanged(paramName,value);
}
