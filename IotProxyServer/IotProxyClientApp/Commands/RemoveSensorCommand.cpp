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

#include "RemoveSensorCommand.h"
#include "../StdQFile.h"
#include <QDebug>

RemoveSensorCommand::RemoveSensorCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool RemoveSensorCommand::evalCommand()
{
	if(parser.getArgs().count()!=2)
	{
		//TODO help
		return false;
	}
	return dev->writeMsg(IClientCommand::removeSensorCommand,parser.getArgs());
}
