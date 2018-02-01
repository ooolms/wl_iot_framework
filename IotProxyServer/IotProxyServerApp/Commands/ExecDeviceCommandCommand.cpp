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

#include "ExecDeviceCommandCommand.h"
#include "../IotProxyInstance.h"
#include "ARpcBase/ARpcSyncCall.h"
#include "StandardErrors.h"

ExecDeviceCommandCommand::ExecDeviceCommandCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool ExecDeviceCommandCommand::processCommand(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.title!="exec_command")return false;
	if(m.args.count()<3)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	bool useSync=(m.args[1]=="1");
	QByteArray cmd=m.args[2];
	QByteArrayList cmdArgs=m.args;
	cmdArgs.removeAt(0);
	cmdArgs.removeAt(0);
	cmdArgs.removeAt(0);
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(m.args[0]);
	if(!dev)
	{
		retVal.append(QByteArray(StandardErrors::noDeviceWithId).replace("%1",m.args[0]));
		return false;
	}
	if(useSync)
	{
		ARpcSyncCall call;
		if(!call.call(dev,cmd,cmdArgs,retVal))
			return false;
	}
	else dev->writeMsg(ARpcConfig::funcCallMsg,QByteArrayList()<<cmd<<cmdArgs);
	return true;
}

QByteArrayList ExecDeviceCommandCommand::acceptedCommands()
{
	return QByteArrayList()<<"exec_command";
}
