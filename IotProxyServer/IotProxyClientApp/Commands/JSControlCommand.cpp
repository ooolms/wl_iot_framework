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

#include "JSControlCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

JSControlCommand::JSControlCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool JSControlCommand::evalCommand()
{
	if(parser.getArgs().count()<1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::jsProgramCommand);
		return false;
	}
	QString subCommand=parser.getArgs()[0];
	if(subCommand=="list")
		return dev->writeMsg("list_js");
	else if(subCommand!="start"&&subCommand!="stop"&&subCommand!="restart")
	{
		StdQFile::inst().stderrDebug()<<"Invalid subcommand\n";
		ShowHelp::showHelp("",IClientCommand::jsProgramCommand);
		return false;
	}
	if(parser.getArgs().count()!=2)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::jsProgramCommand);
		return false;
	}
	QString jsFileName=parser.getArgs()[1];
	return dev->writeMsg(subCommand+"_js",QStringList()<<jsFileName);
}