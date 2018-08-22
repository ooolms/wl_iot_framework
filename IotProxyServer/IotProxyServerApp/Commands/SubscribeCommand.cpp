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

bool SubscribeCommand::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	if(args.count()<2)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=args[0];
	QByteArray sensorName=args[1];
	QUuid devId;
	ARpcISensorStorage *stor=IotProxyInstance::inst().sensorsStorage()->findStorageForDevice(
		devIdOrName,sensorName,devId);
	if(!stor||devId.isNull())
	{
		retVal.append("no sensor found in the local storage");
		return false;
	}
	if(cmd=="subscribe")
		QObject::connect(stor,&ARpcISensorStorage::newValueWritten,proc,&IotProxyCommandProcessor::onNewValueWritten);
	else
		QObject::disconnect(stor,&ARpcISensorStorage::newValueWritten,proc,
			&IotProxyCommandProcessor::onNewValueWritten);
	return true;
}

QByteArrayList SubscribeCommand::acceptedCommands()
{
	return QByteArrayList()<<"subscribe"<<"unsubscribe";
}
