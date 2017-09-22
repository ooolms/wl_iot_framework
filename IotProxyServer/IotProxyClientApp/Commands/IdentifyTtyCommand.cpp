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

#include "IdentifyTtyCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

IdentifyTtyCommand::IdentifyTtyCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool IdentifyTtyCommand::evalCommand()
{
	if(parser.getArgs().count()!=1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::identifyTtyCommand);
		return false;
	}
	return dev->writeMsg(IClientCommand::identifyTtyCommand,parser.getArgs());
}

bool IdentifyTtyCommand::onOk(const QStringList &args)
{
	if(args.count()==2)
	{
		StdQFile::inst().stdoutDebug()<<"Device identified: id="<<args[0]<<" name="<<args[1]<<"\n";
		return true;
	}
	return false;
}
