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

bool ExecDeviceCommandCommand::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	if(cmd!="exec_command")return false;
	if(args.count()<2)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(args[0]);
	QByteArray devCmd=args[1];
	QByteArrayList devCmdArgs=args.mid(2);
	if(!dev)
	{
		retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",args[0]));
		return false;
	}
	ARpcSyncCall call(dev);
	bool ok=call.call(devCmd,devCmdArgs);
	retVal=call.returnValue();
	return ok;
}

QByteArrayList ExecDeviceCommandCommand::acceptedCommands()
{
	return QByteArrayList()<<"exec_command";
}
