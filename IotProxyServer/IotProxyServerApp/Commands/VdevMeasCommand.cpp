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

#include "VdevMeasCommand.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"
#include <QSerialPortInfo>
#include "ARpcBase/ARpcServerConfig.h"

VdevMeasCommand::VdevMeasCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool VdevMeasCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=ctx.args[0];
	QByteArray sensorName=ctx.args[1];
	QByteArrayList data=ctx.args.mid(2);
	ARpcVirtualDevice *vDev=IotProxyInstance::inst().devices()->virtualDeviceByIdOrName(devIdOrName);
	if(!vDev)
	{
		ctx.retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",ctx.args[0]));
		return false;
	}
	QUuid devId=vDev->id();
	DataCollectionUnit *unit=IotProxyInstance::inst().collectionUnit(devId,sensorName);
	if(!unit)
	{
		ctx.retVal.append("sensor for device not found or not in storage");
		return false;
	}
	if(!unit->parseValueFromStrList(data))
	{
		ctx.retVal.append("invalid sensor value, see sensors list");
		return false;
	}
	return true;
}

QByteArrayList VdevMeasCommand::acceptedCommands()
{
	return QByteArrayList()<<"vdev_meas";
}
