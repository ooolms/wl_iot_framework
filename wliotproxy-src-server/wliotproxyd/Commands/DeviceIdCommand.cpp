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

#include "DeviceIdCommand.h"
#include "../ServerInstance.h"
#include "StandardErrors.h"
#include <QSerialPortInfo>
#include "wliot/WLIOTServerProtocolDefs.h"

DeviceIdCommand::DeviceIdCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool DeviceIdCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments());
		return false;
	}
	QByteArray devIdOrName=ctx.args[0];
	QUuid devId=ServerInstance::inst().findDevId(devIdOrName);
	if(!devId.isNull())
	{
		ctx.retVal.append(devId.toByteArray());
		return true;
	}
	ctx.retVal.append(StandardErrors::noDeviceFound(ctx.args[0]));
	return false;
}

QByteArrayList DeviceIdCommand::acceptedCommands()
{
	return QByteArrayList()<<"device_id";
}
