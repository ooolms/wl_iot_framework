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

#include "ListSensorsCommand.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"

ListSensorsCommand::ListSensorsCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool ListSensorsCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(ctx.args[0]);
	if(dev==0)
	{
		ctx.retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",ctx.args[0]));
		return false;
	}
	QList<ARpcSensorDef> sensors;
	if(!dev->getSensorsDescription(sensors))
	{
		ctx.retVal.append("can't get sensors from device");
		return false;
	}
	QByteArray xmlData;
	ARpcSensorDef::dumpToXml(xmlData,sensors);
	ctx.retVal.append(xmlData);
	return true;
}

QByteArrayList ListSensorsCommand::acceptedCommands()
{
	return QByteArrayList()<<"list_sensors";
}
