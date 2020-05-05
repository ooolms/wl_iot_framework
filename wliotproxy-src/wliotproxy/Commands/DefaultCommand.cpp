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

#include "DefaultCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

DefaultCommand::DefaultCommand(const CmdArgParser &p,IotServerConnection *c,const QString &cmd,int minArgsCount)
	:IClientCommand(p,c)
{
	command=cmd;
	minArgs=minArgsCount;
}

bool DefaultCommand::evalCommand()
{
	if(parser.args.count()<minArgs)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",command);
		return false;
	}
	return writeCommandToServer(command.toUtf8(),stringListToByteArrayList(parser.args));
}
