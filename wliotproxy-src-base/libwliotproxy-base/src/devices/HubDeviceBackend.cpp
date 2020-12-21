#include "wliot/devices/HubDeviceBackend.h"
#include "wliot/devices/RealDevice.h"

using namespace WLIOT;

const QByteArray HubDeviceBackend::mBackendType=QByteArray("hub");

HubDeviceBackend::HubDeviceBackend(const QUuid &devId,const QByteArray &name,const QUuid &typeId,RealDevice *pDev)
	:IHighLevelDeviceBackend(pDev)
	,mId(devId)
	,mName(name)
	,mTypeId(typeId)
{
	parentDevice=pDev;
	mSelfConnected=true;
	connect(parentDevice,&RealDevice::connected,this,&HubDeviceBackend::onParentConnected);
	connect(parentDevice,&RealDevice::disconnected,this,&HubDeviceBackend::onParentDisconnected);
}

HubDeviceBackend::~HubDeviceBackend()
{
	setSelfConnected(false);
	emit destroyedBeforeQObject();
}

bool HubDeviceBackend::writeMessageToDevice(const Message &m)
{
	return parentDevice->writeMsgToDevice(
		Message(WLIOTProtocolDefs::hubMsg,QByteArrayList()<<mId.toRfc4122().toHex()<<m.title<<m.args));
}

bool HubDeviceBackend::isConnected()const
{
	return parentDevice->isConnected()&&mSelfConnected;
}

void HubDeviceBackend::forceDisconnect()
{
	if(mSelfConnected)
	{
		setSelfConnected(false);
		emit forceDisconnectCalled();
	}
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

QByteArray HubDeviceBackend::backendType()const
{
	return mBackendType;
}

QString HubDeviceBackend::hwAddress()const
{
	return parentDevice->backend()->hwAddress()+"|"+mId.toString();
}

QUuid HubDeviceBackend::devId()
{
	return mId;
}

QByteArray HubDeviceBackend::devName()
{
	return mName;
}

QUuid HubDeviceBackend::devTypeId()
{
	return mTypeId;
}

void HubDeviceBackend::messageFromDevice(const Message &m)
{
	emit newMessageFromDevice(m);
}
