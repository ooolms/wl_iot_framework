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

#include "IdentifyCommand.h"
#include "../IotProxyConfig.h"


IdentifyCommand::IdentifyCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool IdentifyCommand::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.title!="identify")return false;
	if(IotProxyConfig::serverId.isNull())
	{
		retVal<<"No server id set";
		return false;
	}
	retVal<<IotProxyConfig::serverId.toString()<<IotProxyConfig::serverName;
	return true;
}

QStringList IdentifyCommand::acceptedCommands()
{
	return QStringList()<<"identify";
}