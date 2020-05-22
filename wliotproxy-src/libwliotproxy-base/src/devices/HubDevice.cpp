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

using namespace WLIOT;

HubDevice::HubDevice(const QUuid id,const QByteArray &name,RealDevice *parent)
	:RealDevice(parent)
{
	parentDevice=parent;
	backend=new HubDeviceBackend(parent,this);
	setBackend(backend);
	devId=id;
	devName=name;
}

void HubDevice::setSelfConnected(bool c)
{
	backend->setSelfConnected(c);
}

void HubDevice::syncFailed()
{
	emit internalSyncFailed();
}
