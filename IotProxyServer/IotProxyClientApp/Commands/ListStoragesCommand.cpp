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

#include "ListStoragesCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include "ARpcBase/ARpcServerConfig.h"
#include <QDebug>

ListStoragesCommand::ListStoragesCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool ListStoragesCommand::evalCommand()
{
	return dev->writeMsg(IClientCommand::listStoragesCommand);
}

bool ListStoragesCommand::onCmdData(const QStringList &args)
{
	if(args.count()<5)return false;
	QDebug d=StdQFile::inst().stdoutDebug();
	d<<"Storage:";
	d<<"\n\tdevice id: "<<args[0];
	d<<"\n\tdevice name: "<<args[1];
	d<<"\n\tsensor name: "<<args[2];
	d<<"\n\tstorage type: "<<args[3];
	d<<"\n\ttimestamp transformation rule: "<<args[4]<<"\n";
	return true;
}
