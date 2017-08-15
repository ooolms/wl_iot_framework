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
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

ListIdentifiedCommand::ListIdentifiedCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&ListIdentifiedCommand::onRawMessage);
}

bool ListIdentifiedCommand::evalCommand()
{
	return dev->writeMsg(IClientCommand::listIdentifiedCommand);
}

void ListIdentifiedCommand::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::srvCmdDataMsg)
	{
		if(m.args.count()<4)return;
		QDebug d=StdQFile::inst().stdoutDebug();
		d<<"Identified device:";
		d<<"\n\tuid: "<<m.args[0];
		d<<"\n\tname: "<<m.args[1];
		d<<"\n\tconnection type: "<<m.args[2];
		d<<"\n\tport or address: "<<m.args[3]<<"\n";
	}
}