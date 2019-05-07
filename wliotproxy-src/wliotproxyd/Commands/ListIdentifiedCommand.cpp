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
#include "wliot/devices/TcpSslDevice.h"
#include "wliot/devices/SerialDevice.h"
#include "StandardErrors.h"
#include "wliot/WLIOTServerProtocolDefs.h"

ListIdentifiedCommand::ListIdentifiedCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool ListIdentifiedCommand::processCommand(CallContext &ctx)
{
	IdType uid=proc->uid();

	for(QUuid id:ServerInstance::inst().devices()->identifiedDevicesIds())
	{
		if(!MainServerConfig::accessManager.userCanAccessDevice(id,uid,DevicePolicyActionFlag::ANY))continue;
		RealDevice *dev=ServerInstance::inst().devices()->deviceById(id);
		if(!dev)continue;
		QByteArray devAddr;
		if(dev->metaObject()->className()==SerialDevice::staticMetaObject.className())
			devAddr="tty:"+((SerialDevice*)dev)->portName().toUtf8();
		else if(dev->metaObject()->className()==TcpDevice::staticMetaObject.className())
			devAddr="tcp:"+((TcpDevice*)dev)->address().toUtf8();
		else if(dev->metaObject()->className()==TcpSslDevice::staticMetaObject.className())
			devAddr="tcps:"+((TcpSslDevice*)dev)->address().toUtf8();
		else if(dev->metaObject()->className()==VirtualDevice::staticMetaObject.className())
			devAddr="virtual";

		writeCmdataMsg(ctx.callId,
			QByteArrayList()<<id.toByteArray()<<dev->name()<<dev->typeId().toByteArray()<<devAddr);
	}
	return true;
}

QByteArrayList ListIdentifiedCommand::acceptedCommands()
{
	return QByteArrayList()<<"list_identified";
}
