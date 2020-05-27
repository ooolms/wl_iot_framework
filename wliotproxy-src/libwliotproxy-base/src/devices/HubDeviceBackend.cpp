#include "wliot/devices/HubDeviceBackend.h"
#include "wliot/devices/RealDevice.h"
#include "wliot/devices/HubDevice.h"

using namespace WLIOT;

const QByteArray HubDeviceBackend::devType=QByteArray("hub");

HubDeviceBackend::HubDeviceBackend(RealDevice *pDev,HubDevice *hDev)
	:IHighLevelDeviceBackend(hDev)
{
	parentDevice=pDev;
	hubDev=hDev;
	mSelfConnected=false;
	connect(parentDevice,&RealDevice::connected,this,&HubDeviceBackend::onParentConnected);
	connect(parentDevice,&RealDevice::disconnected,this,&HubDeviceBackend::onParentDisconnected);
}

bool HubDeviceBackend::writeMessageToDevice(const Message &m)
{
	return parentDevice->writeMsgToDevice(
		Message(WLIOTProtocolDefs::hubMsg,QByteArrayList()<<hubDev->id().toRfc4122().toHex()<<m.title<<m.args));
}

bool HubDeviceBackend::isConnected()const
{
	return parentDevice->isConnected()&&mSelfConnected;
}

void HubDeviceBackend::forceDisconnect()
{
	setSelfConnected(false);
}

void HubDeviceBackend::setSelfConnected(bool c)
{
	if(c==mSelfConnected)return;
	mSelfConnected=c;
	if(parentDevice->isConnected())
	{
		if(mSelfConnected)
			emit connected();
		else emit disconnected();
	}
}

void HubDeviceBackend::onParentConnected()
{
	if(mSelfConnected)
		emit connected();
}

void HubDeviceBackend::onParentDisconnected()
{
	if(mSelfConnected)
		emit disconnected();
}

QByteArray HubDeviceBackend::type()const
{
	return devType;
}

QByteArray HubDeviceBackend::portOrAddress() const
{
	return "";
}
