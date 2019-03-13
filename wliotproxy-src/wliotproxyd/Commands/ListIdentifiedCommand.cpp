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

#include "ListIdentifiedCommand.h"
#include "../ServerInstance.h"
#include "../MainServerConfig.h"
#include "wliot/devices/TcpDevice.h"
#include "wliot/devices/SerialDevice.h"
#include "StandardErrors.h"
#include "wliot/WLIOTServerProtocolDefs.h"

ListIdentifiedCommand::ListIdentifiedCommand(QtIODeviceWrap *d,CommandProcessor *p)
	:ICommand(d,p)
{
}

bool ListIdentifiedCommand::processCommand(CallContext &ctx)
{
	IdType uid=proc->uid();
	for(SerialDevice *dev:ServerInstance::inst().devices()->ttyDevices())
	{
		if(dev->isIdentified()&&
			MainServerConfig::accessManager.userCanAccessDevice(dev->id(),uid,DevicePolicyActionFlag::ANY))
				writeCmdataMsg(ctx.callId,
					QByteArrayList()<<dev->id().toByteArray()<<dev->name()<<dev->typeId().toByteArray()<<
						"tty:"+dev->portName().toUtf8());
	}
	for(TcpDevice *dev:ServerInstance::inst().devices()->tcpDevices())
	{
		if(dev->isIdentified()&&
			MainServerConfig::accessManager.userCanAccessDevice(dev->id(),uid,DevicePolicyActionFlag::ANY))
				writeCmdataMsg(ctx.callId,
					QByteArrayList()<<dev->id().toByteArray()<<dev->name()<<dev->typeId().toByteArray()<<
						"tcp:"+dev->address().toUtf8());
	}
	for(VirtualDevice *dev:ServerInstance::inst().devices()->virtualDevices())
	{
		if(dev->isIdentified()&&
			MainServerConfig::accessManager.userCanAccessDevice(dev->id(),uid,DevicePolicyActionFlag::ANY))
				writeCmdataMsg(ctx.callId,
					QByteArrayList()<<dev->id().toByteArray()<<dev->name()<<dev->typeId().toByteArray()<<"");
	}
	for(RealDevice *dev:ServerInstance::inst().devices()->hubDevices())
	{
		if(dev->isIdentified()&&
			MainServerConfig::accessManager.userCanAccessDevice(dev->id(),uid,DevicePolicyActionFlag::ANY))
				writeCmdataMsg(ctx.callId,
					QByteArrayList()<<dev->id().toByteArray()<<dev->name()<<dev->typeId().toByteArray()<<"hub");
	}
	return true;
}

QByteArrayList ListIdentifiedCommand::acceptedCommands()
{
	return QByteArrayList()<<"list_identified";
}
