#include "ARpcHubDevice.h"

ARpcHubDevice::ARpcHubDevice(const QUuid id,const QString &name,ARpcDevice *parent)
	:ARpcDevice(parent)
{
	parentDevice=parent;
	devId=id;
	devName=name;
}

bool ARpcHubDevice::writeMsg(const ARpcMessage &m)
{
	return parentDevice->writeMsg(ARpcConfig::hubMsg,QStringList()<<devId.toString()<<m.title<<m.args);
}

bool ARpcHubDevice::isConnected()
{
	return parentDevice->isConnected();
}
