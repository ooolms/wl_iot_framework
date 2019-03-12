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

#include "wliot/devices/HubDevice.h"
#include "wliot/WLIOTProtocolDefs.h"

HubDevice::HubDevice(const QUuid id,const QByteArray &name,RealDevice *parent)
	:RealDevice(parent)
{
	parentDevice=parent;
	devId=id;
	devName=name;
	mSelfConnected=false;
	connect(parent,&RealDevice::disconnected,this,&HubDevice::onParentDisconnected);
	connect(parent,&RealDevice::connected,this,&HubDevice::onParentConnected);
}

bool HubDevice::writeMsgToDevice(const Message &m)
{
	return parentDevice->writeMsgToDevice(
		Message(WLIOTProtocolDefs::hubMsg,QByteArrayList()<<devId.toRfc4122().toHex()<<m.title<<m.args));
}

void HubDevice::setSelfConnected(bool c)
{
	mSelfConnected=c;
	if(parentDevice->isConnected())
	{
		if(mSelfConnected)
			onConnected();
		else onDisconnected();
	}
}

void HubDevice::syncFailed()
{
	emit internalSyncFailed();
}

void HubDevice::onParentConnected()
{
	if(mSelfConnected)
		onConnected();
}

void HubDevice::onParentDisconnected()
{
	onDisconnected();
}
