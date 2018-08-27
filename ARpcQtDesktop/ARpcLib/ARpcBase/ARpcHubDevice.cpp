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

#include "ARpcHubDevice.h"

ARpcHubDevice::ARpcHubDevice(const QUuid id,const QByteArray &name,ARpcRealDevice *parent)
	:ARpcRealDevice(parent)
{
	parentDevice=parent;
	devId=id;
	devName=name;
	connected=false;
	connect(parent,&ARpcRealDevice::disconnected,this,&ARpcHubDevice::onParentDisconnected);
}

bool ARpcHubDevice::writeMsg(const ARpcMessage &m)
{
	return parentDevice->writeMsg(ARpcConfig::hubMsg,QByteArrayList()<<devId.toRfc4122().toHex()<<m.title<<m.args);
}

bool ARpcHubDevice::isConnected()
{
	return connected&&parentDevice->isConnected();
}

void ARpcHubDevice::setSelfConnected(bool c)
{
	connected=c;
	if(!connected)
	{
		emit disconnected();
		syncTimer.stop();
	}
	else if(!syncTimer.isActive())
	{
		mWasSyncr=true;
		syncTimer.start();
	}
}

void ARpcHubDevice::onRawMessage(const ARpcMessage &m)
{
	emit rawMessage(m);
}

void ARpcHubDevice::syncFailed()
{
	emit internalSyncFailed();
}

void ARpcHubDevice::onParentDisconnected()
{
	emit disconnected();
}
