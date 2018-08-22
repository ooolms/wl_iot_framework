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

bool IdentifyTcpCommand::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	if(args.count()<1||args[0].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString address=QString::fromUtf8(args[0]);
	ARpcTcpDevice *dev=IotProxyInstance::inst().devices()->tcpDeviceByAddress(address);
	if(!dev)
	{
		dev=IotProxyInstance::inst().devices()->addTcpDeviceByAddress(address);
		if(!dev)
		{
			retVal.append(QByteArray(StandardErrors::noDeviceWithId).replace("%1",args[0]));
			return false;
		}
	}
	else dev->disconnectFromHost();
	if(!dev->isConnected())
	{
		dev->reconnect();
		dev->waitForConnected();
	}
	if(!dev->isConnected()||(!dev->isIdentified()&&!dev->identify()))
	{
		retVal.append(StandardErrors::deviceNotIdentified);
		return false;
	}
	retVal<<dev->id().toByteArray()<<dev->name();
	return true;
}

QByteArrayList IdentifyTcpCommand::acceptedCommands()
{
	return QByteArrayList()<<"identify_tcp";
}
