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

#include "wliot/devices/VirtualDevice.h"
#include "wliot/WLIOTProtocolDefs.h"

VirtualDevice::VirtualDevice(const QUuid &id,const QByteArray &name,const QUuid &typeId,QObject *parent)
	:RealDevice(parent)
{
	mId=id;
	clientPointer=0;
	resetIdentification(mId,name,typeId);
}

bool VirtualDevice::writeMsgToDevice(const Message &m)
{
	return QMetaObject::invokeMethod(this,"writeMsgToDeviceQueued",Qt::QueuedConnection,Q_ARG(Message,m));
}

void VirtualDevice::setConnected(bool c)
{
	if(c)
		onConnected();
	else onDisconnected();
}

void VirtualDevice::onMessageFromDevice(const Message &m)
{
	if(m.title==WLIOTProtocolDefs::deviceInfoMsg)
	{
		if(m.args.count()<2)return;
		if(QUuid(m.args[0])!=mId)return;//vdev id can't be changed
	}
	onNewMessage(m);
}

void* VirtualDevice::clientPtr()
{
	return clientPointer;
}

void VirtualDevice::setClientPtr(void *p)
{
	clientPointer=p;
}

void VirtualDevice::writeMsgToDeviceQueued(Message m)
{
	emit messageToDevice(m);
}
