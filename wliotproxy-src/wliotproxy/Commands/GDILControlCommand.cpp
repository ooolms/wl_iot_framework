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

#include "GDILControlCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

//TODO add config options and timers

GDILControlCommand::GDILControlCommand(const CmdArgParser &p,IotServerConnection *c)
	:IClientCommand(p,c)
{
}

bool GDILControlCommand::evalCommand()
{
	if(parser.args.count()<1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::gdilProgramCommand);
		return false;
	}
	QByteArray subCommand=parser.args[0].toUtf8();
	if(subCommand=="list")
		return writeCommandToServer("gdil_list");
	else if(subCommand=="upload")
	{
		if(parser.args.count()!=3)
		{
			StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
			ShowHelp::showHelp("",IClientCommand::gdilProgramCommand);
			return false;
		}
		QByteArray programName=parser.args[1].toUtf8();
		QByteArray text;
		QFile file(parser.args[2]);
		if(!file.open(QIODevice::ReadOnly))
		{
			StdQFile::inst().stderrDebug()<<"can't open program file: "+parser.args[2];
			return false;
		}
		text=file.readAll();
		file.close();
		if(!programName.endsWith(".gdil"))
			programName.append(".gdil");
		return writeCommandToServer("gdil_upload",QByteArrayList()<<programName<<text);
	}
	else if(parser.args[0]=="set_timer")
	{
		if(parser.args.count()<6)return false;
		QByteArrayList otherArgs;
		for(int i=4;i<parser.args.count();++i)
			otherArgs.append(parser.args[i].toUtf8());
		qint64 time=QDateTime::fromString(parser.args[3],Qt::ISODate).toSecsSinceEpoch();
		return writeCommandToServer("gdil_set_timer",QByteArrayList()<<parser.args[1].toUtf8()<<
			parser.args[2].toUtf8()<<QByteArray::number(time)<<otherArgs);
	}
	else if(subCommand!="start"&&subCommand!="stop"&&subCommand!="restart"&&subCommand!="get"&&subCommand!="remove"&&
		subCommand!="list_config_options"&&subCommand!="set_config_option"&&subCommand!="list_timers")
	{
		StdQFile::inst().stderrDebug()<<"Invalid subcommand\n";
		ShowHelp::showHelp("",IClientCommand::gdilProgramCommand);
		return false;
	}
	if(parser.args.count()!=2)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::gdilProgramCommand);
		return false;
	}
	QByteArray jsScriptName=parser.args[1].toUtf8();
	return writeCommandToServer("gdil_"+subCommand,QByteArrayList()<<jsScriptName);
}

bool GDILControlCommand::onOk(const QByteArrayList &args)
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
				StdQFile::inst().stdoutDebug()<<"program: "<<QString::fromUtf8(scripts[i])<<
					"; status: "<<(states[i]?"working":"stopped")<<"\n";
		}
	}
	return true;
}

bool GDILControlCommand::onCmdData(const QByteArrayList &args)
{
	if(parser.args[0]=="list_config_options")
	{
		if(args.count()>=7)
		{
			QDebug d=StdQFile::inst().stdoutDebug();
			d<<"config option: "<<args[0]<<"."<<args[1]<<"; types: "<<args[2]<<"; dim: "<<args[3]<<"\n\tvalue: "<<
				args[4]<<"; dim: "<<args[5]<<"; "<<args.mid(6).join('|')<<"\n";
		}
	}
	else if(parser.args[0]=="gdil_list_timers")
	{
		if(args.count()>=5)
		{
			QDebug d=StdQFile::inst().stdoutDebug();
			QDateTime start=QDateTime::fromSecsSinceEpoch(args[2].toLongLong());
			d<<"timer: "<<args[0]<<"."<<args[1]<<"; start: "<<start.toString(Qt::ISODate)<<
				"; policy: "<<args[3]<<"; repeat interval: "<<args[4];
		}
	}
	return true;
}
