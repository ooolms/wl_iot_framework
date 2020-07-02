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

#include "IdentifyTcpCommand.h"
#include "../MainServerConfig.h"
#include "wliot/devices/TcpDeviceBackend.h"
#include "../ServerInstance.h"
#include "StandardErrors.h"

using namespace WLIOT;

IdentifyTcpCommand::IdentifyTcpCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool IdentifyTcpCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<1||ctx.args[0].isEmpty())
	{
		ctx.retVal.append(StandardErrors::invalidAgruments());
		return false;
	}
	QString address=QString::fromUtf8(ctx.args[0]);
	IHighLevelDeviceBackend *be=ServerInstance::inst().devices()->tcpBackendByAddress(address);
	RealDevice *dev=0;
	if(be)dev=be->device();
	if(dev)
	{
		ctx.retVal<<dev->id().toByteArray()<<dev->name();
		return true;
	}
	ServerInstance::inst().devices()->addOutDevice(TcpDeviceBackend::mBackendType,address,"");
	dev=0;
	be=ServerInstance::inst().devices()->tcpBackendByAddress(address);
	if(be)dev=be->device();
	if(dev)
	{
		ctx.retVal<<dev->id().toByteArray()<<dev->name();
		return true;
	}
	else
	{
		ctx.retVal.append(StandardErrors::noDeviceFound(ctx.args[0]));
		return false;
	}
}

QByteArrayList IdentifyTcpCommand::acceptedCommands()
{
	return QByteArrayList()<<"identify_tcp";
}
