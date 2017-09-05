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
#include "../IotProxyInstance.h"
#include "ARpcDevices/ARpcTcpDevice.h"
#include "ARpcDevices/ARpcTtyDevice.h"
#include "StandardErrors.h"

ListIdentifiedCommand::ListIdentifiedCommand(ARpcOutsideDevice *d)
	:ICommand(d)
{
}

bool ListIdentifiedCommand::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.title!="list_identified")return false;
	for(ARpcTtyDevice *dev:IotProxyInstance::inst().ttyDevices())
	{
		if(dev->isIdentified())clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,
			QStringList()<<dev->id().toString()<<dev->name()<<"tty"<<dev->portName());
	}
	for(ARpcTcpDevice *dev:IotProxyInstance::inst().tcpDevices())
	{
		if(dev->isIdentified())clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,
			QStringList()<<dev->id().toString()<<dev->name()<<"tcp"<<dev->address().toString());
	}
	Q_UNUSED(retVal)
	return true;
}

QStringList ListIdentifiedCommand::acceptedCommands()
{
	return QStringList()<<"list_identified";
}
