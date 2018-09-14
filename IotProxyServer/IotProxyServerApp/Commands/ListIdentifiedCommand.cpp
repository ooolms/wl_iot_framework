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
#include "../IotProxyInstance.h"
#include "ARpcDevices/ARpcTcpDevice.h"
#include "ARpcDevices/ARpcSerialDevice.h"
#include "StandardErrors.h"
#include "ARpcBase/ARpcServerConfig.h"

ListIdentifiedCommand::ListIdentifiedCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool ListIdentifiedCommand::processCommand(CallContext &ctx)
{
	for(ARpcSerialDevice *dev:IotProxyInstance::inst().devices()->ttyDevices())
	{
		if(dev->isIdentified())
			writeCmdataMsg(ctx.callId,
				QByteArrayList()<<dev->id().toByteArray()<<dev->name()<<"tty"<<dev->portName().toUtf8());
	}
	for(ARpcTcpDevice *dev:IotProxyInstance::inst().devices()->tcpDevices())
	{
		if(dev->isIdentified())
			writeCmdataMsg(ctx.callId,
				QByteArrayList()<<dev->id().toByteArray()<<dev->name()<<"tcp"<<dev->address().toUtf8());
	}
	for(ARpcVirtualDevice *dev:IotProxyInstance::inst().devices()->virtualDevices())
	{
		if(dev->isIdentified())
			writeCmdataMsg(ctx.callId,
				QByteArrayList()<<dev->id().toByteArray()<<dev->name()<<"virtual"<<"");
	}
	for(ARpcRealDevice *dev:IotProxyInstance::inst().devices()->hubDevices())
	{
		if(dev->isIdentified())
			writeCmdataMsg(ctx.callId,
				QByteArrayList()<<dev->id().toByteArray()<<dev->name()<<"hub"<<"");
	}
	return true;
}

QByteArrayList ListIdentifiedCommand::acceptedCommands()
{
	return QByteArrayList()<<"list_identified";
}
