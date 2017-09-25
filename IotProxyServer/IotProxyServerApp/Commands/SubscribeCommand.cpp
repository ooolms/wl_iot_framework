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

#include "SubscribeCommand.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"

SubscribeCommand::SubscribeCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool SubscribeCommand::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.args.count()<2)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString devIdOrName=m.args[0];
	QString sensorName=m.args[1];
	ARpcRealDevice *dev=IotProxyInstance::inst().deviceByIdOrName(devIdOrName);
	if(!dev)
	{
		retVal.append(StandardErrors::noDeviceWithId);
		return false;
	}
	QUuid devId=dev->id();
	ARpcISensorStorage *stor=IotProxyInstance::inst().sensorsStorage()->existingStorage({devId,sensorName});
	if(!stor)
	{
		retVal.append("no sensor found in the local storage");
		return false;
	}
	if(m.title=="subscribe")
		QObject::connect(stor,&ARpcISensorStorage::newValueWritten,proc,&IotProxyCommandProcessor::onNewValueWritten);
	else
		QObject::disconnect(stor,&ARpcISensorStorage::newValueWritten,proc,
			&IotProxyCommandProcessor::onNewValueWritten);
	return true;
}

QStringList SubscribeCommand::acceptedCommands()
{
	return QStringList()<<"subscribe"<<"unsubscribe";
}
