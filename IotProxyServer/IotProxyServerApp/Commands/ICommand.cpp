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

#include "ICommand.h"
#include "ARpcBase/ARpcServerConfig.h"

ICommand::ICommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
{
	clientDev=d;
	proc=p;
}

bool ICommand::processCommand(
	const QByteArray &cmd,const QByteArray &cId,const QByteArrayList &args,QByteArrayList &retVal)
{
	callId=cId;
	bool ok=processCommand(cmd,args,retVal);
	retVal.prepend(cId);
	return ok;
}

void ICommand::writeCmdataMsg(const QByteArrayList &args)
{
	clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,QByteArrayList()<<callId<<args);
}
