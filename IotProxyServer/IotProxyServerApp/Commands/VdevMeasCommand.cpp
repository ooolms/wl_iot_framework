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

bool VdevMeasCommand::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.title!="vdev_meas")return false;
	if(m.args.count()<3)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString devIdOrName=m.args[0];
	QString sensorName=m.args[1];
	QStringList data=m.args.mid(2);
	ARpcVirtualDevice *vDev=IotProxyInstance::inst().virtualDeviceByIdOrName(devIdOrName);
	if(!vDev)
	{
		retVal.append(StandardErrors::noDeviceWithId);
		return false;
	}
	QUuid devId=vDev->id();
	DataCollectionUnit *unit=IotProxyInstance::inst().collectionUnit({devId,sensorName});
	if(!unit)
	{
		retVal.append("sensor for device not found or not in storage");
		return false;
	}
	if(!unit->parseValueFromStrList(data))
	{
		retVal.append("invalid sensor value, see sensors list");
		return false;
	}
	return true;
}

QStringList VdevMeasCommand::acceptedCommands()
{
	return QStringList()<<"vdev_meas";
}
