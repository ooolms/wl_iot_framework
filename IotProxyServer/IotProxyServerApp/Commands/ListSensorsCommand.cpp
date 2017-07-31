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

ListSensorsCommand::ListSensorsCommand(ARpcOutsideDevice *d)
	:ICommand(d)
{
}

bool ListSensorsCommand::processCommand(const ARpcMessage &m)
{
	if(m.title=="list_sensors")
	{
		if(m.args.count()<1)
		{
			lastErrorStr=StandardErrors::invalidAgruments;
			return false;
		}
		ARpcDevice *dev=IotProxyInstance::inst().deviceByIdOrName(m.args[0]);
		if(dev==0)
		{
			lastErrorStr=StandardErrors::noDeviceWithId.arg(m.args[0]);
			return false;
		}
		QList<ARpcSensor> sensors;
		if(!dev->getSensorsDescription(sensors))
		{
			lastErrorStr="can't get sensors from device";
			return false;
		}
		QString xmlData;
		ARpcSensor::dumpToXml(xmlData,sensors);
		clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,QStringList()<<xmlData);
		return true;
	}
	return false;
}

QStringList ListSensorsCommand::acceptedCommands()
{
	return QStringList()<<"list_sensors";
}

