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
#include "../include-private/VirtualDeviceBackend.h"
#include "wliot/WLIOTProtocolDefs.h"

using namespace WLIOT;

VirtualDevice::VirtualDevice(const QUuid &id,const QByteArray &name,const QUuid &typeId,QObject *parent)
	:RealDevice(parent)
{
	devId=id;
	devName=name;
	devTypeId=typeId;
	clientPointer=nullptr;
	virtualBackend=new VirtualDeviceBackend(id,name,typeId,this);
	setBackend(virtualBackend);
	connect(virtualBackend,SIGNAL(messageToDevice(WLIOT::Message)),this,SIGNAL(messageToDevice(WLIOT::Message)));
}

void VirtualDevice::setConnected(bool c)
{
	virtualBackend->setConnected(c);
}

void VirtualDevice::emulateMessageFromDevice(const Message &m)
{
	virtualBackend->emulateMessageFromDevice(m);
}

void VirtualDevice::setBackend(IHighLevelDeviceBackend *b)
{
	RealDevice::setBackend(b);
}

IHighLevelDeviceBackend *VirtualDevice::takeBackend()
{
	return RealDevice::takeBackend();
}

void* VirtualDevice::clientPtr()
{
	return clientPointer;
}

void VirtualDevice::setClientPtr(void *p)
{
	clientPointer=p;
}
