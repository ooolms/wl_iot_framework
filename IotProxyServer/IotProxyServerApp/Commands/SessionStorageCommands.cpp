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

#include "SessionStorageCommands.h"
#include "../IotProxyInstance.h"
#include "ARpcLocalStorage/ARpcSessionStorage.h"
#include "ARpcBase/ARpcServerConfig.h"

SessionStorageCommands::SessionStorageCommands(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool SessionStorageCommands::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.title!="session_list")
		return false;
	if(m.args.count()<2)
	{
		retVal.append("invalid arguments");
		return false;
	}
	QUuid devId;
	ARpcISensorStorage *stor=IotProxyInstance::inst().sensorsStorage()->findStorageForDevice(m.args[0],m.args[1],devId);
	if(!stor)
	{
		retVal.append("storage not found");
		return false;
	}
	if(stor->getStoreMode()!=ARpcISensorStorage::MANUAL_SESSIONS&&
		stor->getStoreMode()!=ARpcISensorStorage::AUTO_SESSIONS)
	{
		retVal.append("not a session storage");
		return false;
	}
	ARpcSessionStorage *sStor=(ARpcSessionStorage*)stor;
	if(!sStor->isOpened())
		sStor->open();
	QList<QUuid> ids;
	QStringList titles;
	if(!sStor->listSessions(ids,titles))
	{
		retVal.append("can't list sessions");
		return false;
	}
	for(int i=0;i<ids.count();++i)
		clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,QStringList()<<ids[i].toString()<<titles[i]);
	return true;
}

QStringList SessionStorageCommands::acceptedCommands()
{
	return QStringList()<<"session_list";
}
