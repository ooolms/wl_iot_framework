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

#include "ChangeDeviceOwnerCommand.h"
#include "../IotProxyInstance.h"
#include "../IotProxyConfig.h"
#include "wliot/devices/CommandCall.h"
#include "StandardErrors.h"

ChangeDeviceOwnerCommand::ChangeDeviceOwnerCommand(QtIODeviceWrap *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool ChangeDeviceOwnerCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=ctx.args[0];
	QUuid devId(devIdOrName);
	if(devId.isNull())
	{
		RealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(devIdOrName);
		if(!dev)
		{
			devId=IotProxyInstance::inst().storages()->findDeviceId(devIdOrName);
			if(devId.isNull())
			{
				ctx.retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",devIdOrName));
				return false;
			}
		}
		else devId=dev->id();
	}
	IdType uid=proc->uid();
	IdType newOwnerId=proc->uid();
	if(ctx.args.count()>1)
	{
		QByteArray userName=ctx.args[1];
		newOwnerId=IotProxyConfig::accessManager.userId(userName);
		if(newOwnerId==nullId)
		{
			ctx.retVal.append(QByteArray(StandardErrors::noUserFound).replace("%1",userName));
			return false;
		}
	}
	if(IotProxyConfig::accessManager.userCanChangeDeviceOwner(devId,uid))
	{
		IotProxyConfig::accessManager.setDevOwner(devId,newOwnerId);
		return true;
	}
	ctx.retVal.append("No rights to change device owner");
	return false;
}

QByteArrayList ChangeDeviceOwnerCommand::acceptedCommands()
{
	return QByteArrayList()<<"change_device_owner";
}
