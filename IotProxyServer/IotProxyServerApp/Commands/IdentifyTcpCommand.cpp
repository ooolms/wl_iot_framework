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
#include "../IotProxyConfig.h"
#include "ARpcDevices/ARpcTcpDevice.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"

IdentifyTcpCommand::IdentifyTcpCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool IdentifyTcpCommand::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.args.count()<1||m.args[0].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcTcpDevice *dev=IotProxyInstance::inst().devices()->tcpDeviceByAddress(m.args[0]);
	if(!dev)
	{
		dev=IotProxyInstance::inst().devices()->addTcpDeviceByAddress(m.args[0]);
		if(!dev)
		{
			retVal.append(StandardErrors::noDeviceWithId.arg(m.args[0]));
			return false;
		}
	}
	if(!dev->isConnected())
		dev->waitForConnected();
	if(!dev->isConnected()||(!dev->isIdentified()&&!dev->identify()))
	{
		retVal.append(StandardErrors::deviceNotIdentified);
		return false;
	}
	retVal<<dev->id().toString()<<dev->name();
	return true;
}

QStringList IdentifyTcpCommand::acceptedCommands()
{
	return QStringList()<<"identify_tcp";
}
