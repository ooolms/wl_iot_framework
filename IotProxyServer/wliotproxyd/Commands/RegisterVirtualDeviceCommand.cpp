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

bool RegisterVirtualDeviceCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid deviceId(ctx.args[0]);
	QByteArray devName(ctx.args[1]);
	QList<ARpcSensorDef> sensors;
	ARpcControlsGroup controls;
	bool ok;
	if(ctx.args[2].startsWith('{'))
		ok=ARpcSensorDef::parseJsonDescription(ctx.args[2],sensors);
	else ok=ARpcSensorDef::parseXmlDescription(ctx.args[2],sensors);
	if(!ok)
	{
		ctx.retVal.append("invalid sensors description");
		return false;
	}
	if(ctx.args.count()>=4)
	{
		if(ctx.args[3].startsWith('{'))
			ok=ARpcControlsGroup::parseJsonDescription(ctx.args[3],controls);
		else ok=ARpcControlsGroup::parseXmlDescription(ctx.args[3],controls);
	}
	ARpcVirtualDevice *dev=IotProxyInstance::inst().devices()->registerVirtualDevice(deviceId,devName,sensors,controls);
	if(!dev)
	{
		ctx.retVal.append("can't register virtual device");
		return false;
	}
	proc->registerVDevForCommandsProcessing(dev);
	return true;
}

QByteArrayList RegisterVirtualDeviceCommand::acceptedCommands()
{
	return QByteArrayList()<<"register_virtual_device";
}
