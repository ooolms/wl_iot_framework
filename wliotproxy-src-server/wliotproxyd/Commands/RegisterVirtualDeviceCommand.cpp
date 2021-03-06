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

#include "RegisterVirtualDeviceCommand.h"
#include "../ServerInstance.h"
#include "../MainServerConfig.h"
#include "StandardErrors.h"

using namespace WLIOT;

RegisterVirtualDeviceCommand::RegisterVirtualDeviceCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool RegisterVirtualDeviceCommand::processCommand(CallContext &ctx)
{
	if(ctx.cmd=="register_virtual_device")
		return registerVDev(ctx);
	else if(ctx.cmd=="disconnect_virtual_device")
		return disconnectVDev(ctx);
	else return false;
}

QByteArrayList RegisterVirtualDeviceCommand::acceptedCommands()
{
	return QByteArrayList()<<"register_virtual_device"<<"disconnect_virtual_device";
}

bool RegisterVirtualDeviceCommand::registerVDev(ICommand::CallContext &ctx)
{
	if(ctx.args.count()<2)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments());
		return false;
	}
	QUuid deviceId(ctx.args[0]);
	QByteArray devName(ctx.args[1]);
	QUuid typeId;
	if(ctx.args.count()>2)
		typeId=QUuid(ctx.args[2]);
	if(deviceId.isNull()||devName.isEmpty())
	{
		ctx.retVal.append(StandardErrors::invalidAgruments());
		return false;
	}
	if(!MainServerConfig::accessManager.userCanRegisterVirtualDevice(deviceId,proc->uid()))
	{
		ctx.retVal.append(StandardErrors::accessDenied());
		return false;
	}
	VirtualDeviceBackend *be=new VirtualDeviceBackend(deviceId,devName,typeId,proc);
	proc->registerVDev(be);
	RealDevice *dev=ServerInstance::inst().devices()->registerVirtualDevice(be);
	if(!dev)
	{
		ctx.retVal.append("virtual device already registered");
		return false;
	}
	if(MainServerConfig::accessManager.devOwner(deviceId)==nullId)
		MainServerConfig::accessManager.setDevOwner(deviceId,proc->uid());
	return true;
}

bool RegisterVirtualDeviceCommand::disconnectVDev(ICommand::CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments());
		return false;
	}
	QUuid deviceId(ctx.args[0]);
	if(!proc->hasVDev(deviceId))
	{
		ctx.retVal.append("Virtual device wan not registered");
		return false;
	}
	proc->disconnectVDev(deviceId);
	return true;
}
