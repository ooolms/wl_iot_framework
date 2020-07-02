/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "wliot/devices/VirtualDeviceBackend.h"
#include "wliot/WLIOTProtocolDefs.h"

using namespace WLIOT;

const QByteArray VirtualDeviceBackend::mBackendType=QByteArray("virtual");

VirtualDeviceBackend::VirtualDeviceBackend(const QUuid &id,const QByteArray &name,
	const QUuid &typeId,IVirtualDeviceBackendCallback *cb,QObject *parent)
	:IHighLevelDeviceBackend(parent)
{
	mDevId=id;
	mDevName=name;
	mDevTypeId=typeId;
	mMsgCb=cb;
}

VirtualDeviceBackend::~VirtualDeviceBackend()
{
	emit destroyedBeforeQObject();
}

void VirtualDeviceBackend::emulateMessageFromDevice(const Message &m)
{
	emit newMessageFromDevice(m);
}

QUuid VirtualDeviceBackend::devId()const
{
	return mDevId;
}

IVirtualDeviceBackendCallback* VirtualDeviceBackend::msgCb()
{
	return mMsgCb;
}

bool VirtualDeviceBackend::writeMessageToDevice(const Message &m)
{
	if(!mMsgCb)return false;
	return QMetaObject::invokeMethod(this,"writeMsgToDeviceQueued",Qt::QueuedConnection,Q_ARG(WLIOT::Message,m));
}

bool VirtualDeviceBackend::isConnected()const
{
	return mMsgCb!=0;
}

void VirtualDeviceBackend::forceDisconnect()
{
	mMsgCb=0;
	emit disconnected();
}

QByteArray VirtualDeviceBackend::backendType()const
{
	return mBackendType;
}

QString VirtualDeviceBackend::hwAddress()const
{
	return "virtual:"+mDevId.toString();
}

void VirtualDeviceBackend::writeMsgToDeviceQueued(Message m)
{
	if(m.title==WLIOTProtocolDefs::devSyncMsg)
		emit newMessageFromDevice(Message(WLIOTProtocolDefs::devSyncrMsg,m.args));
	else if(m.title==WLIOTProtocolDefs::identifyMsg)
		emit newMessageFromDevice(Message(WLIOTProtocolDefs::deviceInfoMsg,
			QByteArrayList()<<mDevId.toByteArray()<<mDevName<<mDevTypeId.toByteArray()));
	else if(mMsgCb)
		mMsgCb->onMessageToVDev(this,m);
}
