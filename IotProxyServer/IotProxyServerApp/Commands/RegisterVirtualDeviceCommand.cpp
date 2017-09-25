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

#include "RegisterVirtualDeviceCommand.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"

RegisterVirtualDeviceCommand::RegisterVirtualDeviceCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool RegisterVirtualDeviceCommand::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.title!="register_virtual_device")return false;
	if(m.args.count()<3)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid deviceId(m.args[0]);
	QString devName(m.args[1]);
	QList<ARpcSensor> sensors;
	bool ok;
	if(m.args[2].startsWith('{'))
		ok=ARpcSensor::parseJsonDescription(m.args[2],sensors);
	else ok=ARpcSensor::parseXmlDescription(m.args[2],sensors);
	if(!ok)
	{
		retVal.append("invalid sensors description");
		return false;
	}
	ARpcVirtualDevice *dev=IotProxyInstance::inst().registerVirtualDevice(deviceId,devName,sensors);
	if(!dev)
	{
		retVal.append("can't register virtual device");
		return false;
	}
	return true;
}

QStringList RegisterVirtualDeviceCommand::acceptedCommands()
{
	return QStringList()<<"register_virtual_device";
}
