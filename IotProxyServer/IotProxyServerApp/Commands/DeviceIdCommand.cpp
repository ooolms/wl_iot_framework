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
#include "../IotProxyInstance.h"
#include "StandardErrors.h"
#include <QSerialPortInfo>
#include "ARpcBase/ARpcServerConfig.h"

DeviceIdCommand::DeviceIdCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool DeviceIdCommand::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	if(cmd!="device_id")return false;
	if(args.count()<1)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=args[0];
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(devIdOrName);
	if(dev)
	{
		retVal.append(dev->id().toByteArray());
		return true;
	}
	QUuid devId=IotProxyInstance::inst().sensorsStorage()->findDeviceId(devIdOrName);
	if(!devId.isNull())
	{
		retVal.append(devId.toByteArray());
		return true;
	}
	retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",args[0]));
	return false;
}

QByteArrayList DeviceIdCommand::acceptedCommands()
{
	return QByteArrayList()<<"device_id";
}
