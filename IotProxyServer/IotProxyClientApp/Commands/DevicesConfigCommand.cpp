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

#include "DevicesConfigCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

DevicesConfigCommand::DevicesConfigCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool DevicesConfigCommand::evalCommand()
{
	if(parser.getArgs().count()<1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("","devices_config");
		return false;
	}
	subCommand=parser.getArgs()[0];
	return dev->writeMsg(IClientCommand::devicesConfigCommand,parser.getArgs());
}

bool DevicesConfigCommand::onOk(const QStringList &args)
{
	if(!args.isEmpty())StdQFile::inst().stdoutDebug()<<args.join("|")<<"\n";
	return true;
}
