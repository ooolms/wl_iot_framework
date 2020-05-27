#include "../include-private/VirtualDeviceBackend.h"
#include "wliot/devices/VirtualDevice.h"

using namespace WLIOT;

const QByteArray VirtualDeviceBackend::devType=QByteArray("virtual");

VirtualDeviceBackend::VirtualDeviceBackend(const QUuid &id,const QByteArray &name,const QUuid &typeId,QObject *parent)
	:IHighLevelDeviceBackend(parent)
{
	vDevId=id;
	vDevName=name;
	vDevTypeId=typeId;
	mConnected=false;
}

void VirtualDeviceBackend::emulateMessageFromDevice(const Message &m)
{
	emit newMessageFromDevice(m);
}

void VirtualDeviceBackend::setConnected(bool c)
{
	if(c==mConnected)return;
	mConnected=c;
	if(mConnected)
		emit connected();
	else emit disconnected();
}


bool VirtualDeviceBackend::writeMessageToDevice(const Message &m)
{
	if(!mConnected)return false;
	return QMetaObject::invokeMethod(this,"writeMsgToDeviceQueued",Qt::QueuedConnection,Q_ARG(WLIOT::Message,m));
}

bool VirtualDeviceBackend::isConnected()const
{
	return mConnected;
}

void VirtualDeviceBackend::forceDisconnect()
{
	mConnected=false;
	emit disconnected();
}

QByteArray VirtualDeviceBackend::type()const
{
	return devType;
}

QByteArray VirtualDeviceBackend::portOrAddress()const
{
	return "";
}

void VirtualDeviceBackend::writeMsgToDeviceQueued(Message m)
{
	if(m.title==WLIOTProtocolDefs::devSyncMsg)
		emit newMessageFromDevice(Message(WLIOTProtocolDefs::devSyncrMsg,m.args));
	else if(m.title==WLIOTProtocolDefs::identifyMsg)
		emit newMessageFromDevice(Message(WLIOTProtocolDefs::deviceInfoMsg,
			QByteArrayList()<<vDevId.toRfc4122()<<vDevName<<vDevTypeId.toRfc4122()));
	else emit messageToDevice(m);
}
