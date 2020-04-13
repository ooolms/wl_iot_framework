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

JSControlCommand::JSControlCommand(const CmdArgParser &p,IotServerConnection *c)
	:IClientCommand(p,c)
{
}

bool JSControlCommand::evalCommand()
{
	if(parser.args.count()<1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::jsProgramCommand);
		return false;
	}
	QByteArray subCommand=parser.args[0].toUtf8();
	if(subCommand=="list")
		return writeCommandToServer("js_list");
	else if(subCommand=="upload")
	{
		if(parser.args.count()!=3)
		{
			StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
			ShowHelp::showHelp("",IClientCommand::jsProgramCommand);
			return false;
		}
		QByteArray jsScriptName=parser.args[1].toUtf8();
		QByteArray text;
		QFile file(parser.args[2]);
		if(!file.open(QIODevice::ReadOnly))
		{
			StdQFile::inst().stderrDebug()<<"can't open script file: "+parser.args[2];
			return false;
		}
		text=file.readAll();
		file.close();
		if(!jsScriptName.endsWith(".js"))
			jsScriptName.append(".js");
		return writeCommandToServer("js_upload",QByteArrayList()<<jsScriptName<<text);
	}
	else if(subCommand!="start"&&subCommand!="stop"&&subCommand!="restart"&&subCommand!="get"&&subCommand!="remove")
	{
		StdQFile::inst().stderrDebug()<<"Invalid subcommand\n";
		ShowHelp::showHelp("",IClientCommand::jsProgramCommand);
		return false;
	}
	if(parser.args.count()!=2)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::jsProgramCommand);
		return false;
	}
	QByteArray jsScriptName=parser.args[1].toUtf8();
	return writeCommandToServer("js_"+subCommand,QByteArrayList()<<jsScriptName);
}

bool JSControlCommand::onOk(const QByteArrayList &args)
{
	if(parser.args[0]=="get"&&args.count()>0)
		StdQFile::inst().stdoutDebug()<<QString::fromUtf8(args[0]);
	else if(parser.args[0]=="list")
	{
		if(args.count()%2!=0)return false;
		QByteArrayList scripts;
		QList<bool> states;
		for(int i=0;i<args.count()>>1;++i)
		{
			scripts.append(args[i<<1]);
			states.append(args[(i<<1)+1]=="1");
		}
		if(mForCompletion)
			StdQFile::inst().stdoutDebug()<<QString::fromUtf8(scripts.join(" "));
		else
		{
			for(int i=0;i<scripts.count();++i)
				StdQFile::inst().stdoutDebug()<<"script: "<<QString::fromUtf8(scripts[i])<<
					"; status: "<<(states[i]?"working":"stopped");
		}
	}
	return true;
}
