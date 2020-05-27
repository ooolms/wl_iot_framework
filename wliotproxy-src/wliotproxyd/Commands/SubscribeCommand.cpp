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
#include "../ServerInstance.h"
#include "StandardErrors.h"

using namespace WLIOT;

SubscribeCommand::SubscribeCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool SubscribeCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<2)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=ctx.args[0];
	QByteArray sensorName=ctx.args[1];
	QUuid devId;
	ISensorStorage *stor=ServerInstance::inst().storages()->findStorageForDevice(
		devIdOrName,sensorName,devId);
	if(!stor||devId.isNull())
	{
		ctx.retVal.append("no sensor found in the local storage");
		return false;
	}
	if(ctx.cmd=="subscribe")
		QObject::connect(stor,SIGNAL(newValueWritten(const WLIOT::SensorValue*)),
			proc,SLOT(CommandProcessor::onNewValueWritten(const WLIOT::SensorValue*)));
	else
		QObject::disconnect(stor,SIGNAL(newValueWritten(const WLIOT::SensorValue*)),
			proc,SLOT(CommandProcessor::onNewValueWritten(const WLIOT::SensorValue*)));
	return true;
}

QByteArrayList SubscribeCommand::acceptedCommands()
{
	return QByteArrayList()<<"subscribe"<<"unsubscribe";
}
