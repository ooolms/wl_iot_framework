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
	if(m.args.count()<1)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QHostAddress address=QHostAddress(m.args[0]);
	if(address.isNull())
	{
		retVal.append("Invalid IP or hostname");
		return false;
	}
	ARpcTcpDevice *dev=IotProxyInstance::inst().devices()->tcpDeviceByAddress(address);
	if(!dev)
	{
		dev=IotProxyInstance::inst().devices()->addTcpDeviceByAddress(address);
		if(!dev)
		{
			retVal.append(StandardErrors::noDeviceWithId.arg(address.toString()));
			return false;
		}
	}
	if(!dev->isIdentified()&&!dev->identify())
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
