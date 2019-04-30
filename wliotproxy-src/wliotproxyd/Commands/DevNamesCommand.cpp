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

#include "DevNamesCommand.h"
#include "../ServerInstance.h"
#include "../MainServerConfig.h"
#include "wliot/devices/CommandCall.h"
#include "wliot/WLIOTServerProtocolDefs.h"
#include "StandardErrors.h"
#include "wliot/storages/AllStorages.h"

DevNamesCommand::DevNamesCommand(QtIODeviceWrap *d,CommandProcessor *p)
	:ICommand(d,p)
{
}

bool DevNamesCommand::processCommand(CallContext &ctx)
{
	IDevicesNamesDatabase *db=ServerInstance::inst().devNames();
	if(ctx.cmd=="dev_names")
	{
		for(const QByteArray &uidStr:ctx.args)
			ctx.retVal.append(db->deviceName(QUuid(uidStr)));
	}
	else if(ctx.cmd=="set_dev_name")
	{
		if(ctx.args.count()<1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QUuid devId=ServerInstance::inst().findDevId(ctx.args[0]);
		QByteArray devName;
		if(ctx.args.count()>1)
			devName=ctx.args[1];
		if(devId.isNull())
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		if(proc->uid()==rootUid||proc->uid()==MainServerConfig::accessManager.devOwner(devId))
		{
			if(!ServerInstance::inst().devNames()->setManualDevName(devId,devName))
				return false;
			RealDevice *dev=ServerInstance::inst().devices()->deviceById(devId);
			if(dev)dev->forceRename(devName,false);
			return true;
		}
		else
		{
			ctx.retVal.append(StandardErrors::accessDenied);
			return false;
		}
	}
	return false;
}

QByteArrayList DevNamesCommand::acceptedCommands()
{
	return QByteArrayList()<<"dev_names"<<"set_dev_name";
}
