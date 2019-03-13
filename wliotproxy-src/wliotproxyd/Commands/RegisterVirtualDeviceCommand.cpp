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

RegisterVirtualDeviceCommand::RegisterVirtualDeviceCommand(QtIODeviceWrap *d,CommandProcessor *p)
	:ICommand(d,p)
{
}

bool RegisterVirtualDeviceCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<2)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid deviceId(ctx.args[0]);
	QByteArray devName(ctx.args[1]);
	QUuid typeId;
	if(ctx.args.count()>2)
		typeId=QUuid(ctx.args[2]);
	VirtualDevice *dev=ServerInstance::inst().devices()->registerVirtualDevice(deviceId,devName,typeId);
	if(!dev)
	{
		ctx.retVal.append("virtual device already registered");
		return false;
	}
	if(dev->clientPtr())
	{
		if(dev->clientPtr()==proc)
			return true;
		else return false;
	}
	IdType ownerId=MainServerConfig::accessManager.devOwner(deviceId);
	if(ownerId!=nullId)
	{
		if(ownerId!=proc->uid()&&proc->uid()!=rootUid)
		{
			ctx.retVal.append(StandardErrors::accessDenied);
			return false;
		}
	}
	proc->registerVDevForCommandsProcessing(dev);
	return true;
}

QByteArrayList RegisterVirtualDeviceCommand::acceptedCommands()
{
	return QByteArrayList()<<"register_virtual_device";
}
