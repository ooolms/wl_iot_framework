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

#include "SessionCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

SessionCommand::SessionCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool SessionCommand::evalCommand()
{
	static const QMap<QByteArray,int> paramsCounts=QMap<QByteArray,int>(
	{
		{"list",2},
		{"list_attrs",3},
		{"get_attr",4},
		{"set_attr",4},
		{"start",3},
		{"stop",2},
		{"continue",3},
		{"get_write_id",2}
	});

	if(parser.getArgs().count()<1)
	{
		StdQFile::inst().stderrDebug()<<"No subcommand\n";
		ShowHelp::showHelp("",IClientCommand::sessionCommand);
		return false;
	}
	QByteArray subCommand=parser.getArgs()[0].toUtf8();
	QByteArrayList subCommandParams;
	for(int i=1;i<parser.getArgs().count();++i)
		subCommandParams.append(parser.getArgs()[i].toUtf8());
	if(!paramsCounts.contains(subCommand))
	{
		StdQFile::inst().stderrDebug()<<"Invalid subcommand\n";
		ShowHelp::showHelp("",IClientCommand::sessionCommand);
		return false;
	}
	if(subCommandParams.count()<paramsCounts[subCommand])
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::sessionCommand);
		return false;
	}
	return dev->writeMsg("session_"+subCommand,subCommandParams);
}
