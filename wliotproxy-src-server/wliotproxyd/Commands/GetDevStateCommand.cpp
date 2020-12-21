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

#include "GetDevStateCommand.h"
#include "../ServerInstance.h"
#include "StandardErrors.h"

using namespace WLIOT;

GetDevStateCommand::GetDevStateCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool GetDevStateCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments());
		return false;
	}
	RealDevice *dev=ServerInstance::inst().devices()->deviceByIdOrName(ctx.args[0]);
	if(dev==0)
	{
		ctx.retVal.append(StandardErrors::noDeviceFound(ctx.args[0]));
		return false;
	}
	DeviceState state;
	if(!dev->getState(state))
	{
		ctx.retVal.append("can't get state from device");
		return false;
	}
	ctx.retVal.append(state.dumpToMsgArgs());
	return true;
}

QByteArrayList GetDevStateCommand::acceptedCommands()
{
	return QByteArrayList()<<"get_dev_state";
}
