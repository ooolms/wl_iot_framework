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

#include "IExternCommandSource.h"
#include "ServerInstance.h"
#include "wliot/devices/CommandCall.h"
#include "ServerLogs.h"
#include <QDebug>

using namespace WLIOT;

IExternCommandSource::IExternCommandSource(QObject *parent)
	:QObject(parent)
{
}

bool IExternCommandSource::execCommand(const QByteArray &devIdOrName,const QByteArray &command,
	const QByteArrayList &arguments,QByteArrayList &retVal)
{
	RealDevice *dev=ServerInstance::inst().devices()->deviceByIdOrName(devIdOrName);
	if(!dev)return false;
	ServerLogs::logDevices(QtInfoMsg,
		"Exec command on device: "+dev->id().toByteArray()+"; "+command+"|"+arguments.join("|"));
	QSharedPointer<CommandCall> call=dev->execCommand(command,arguments);
	bool ok=call->wait();
	retVal=call->returnValue();
	return ok;
}

