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

#include "BindSensorCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

BindSensorCommand::BindSensorCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool BindSensorCommand::evalCommand()
{
	if(parser.getArgs().count()<3)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("","bind_sensor");
		return false;
	}
	return dev->writeMsg(IClientCommand::bindSensorCommand,parser.getArgs());
}
