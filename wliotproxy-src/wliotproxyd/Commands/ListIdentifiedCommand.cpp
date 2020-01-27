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

#include "ListIdentifiedCommand.h"
#include "../ServerInstance.h"
#include "../MainServerConfig.h"
#include "wliot/devices/TcpDeviceBackend.h"
#include "wliot/devices/TcpSslDeviceBackend.h"
#include "wliot/devices/SerialDeviceBackend.h"
#include "StandardErrors.h"
#include "wliot/WLIOTServerProtocolDefs.h"

ListIdentifiedCommand::ListIdentifiedCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool ListIdentifiedCommand::processCommand(CallContext &ctx)
{
	IdType uid=proc->uid();

	for(QUuid id:ServerInstance::inst().devices()->identifiedDevicesIds())
	{
		if(!MainServerConfig::accessManager.userCanAccessDevice(id,uid,DevicePolicyActionFlag::ANY))continue;
		RealDevice *dev=ServerInstance::inst().devices()->deviceById(id);
		if(!dev)continue;
		writeCmdataMsg(ctx.callId,
			QByteArrayList()<<id.toByteArray()<<dev->name()<<dev->classId().toByteArray()<<dev->backend()->type()<<
				dev->backend()->portOrAddress());
	}
	return true;
}

QByteArrayList ListIdentifiedCommand::acceptedCommands()
{
	return QByteArrayList()<<"list_identified";
}
