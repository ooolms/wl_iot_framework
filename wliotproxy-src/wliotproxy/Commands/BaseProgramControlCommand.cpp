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

#include "BaseProgramControlCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

using namespace WLIOTClient;

BaseProgramControlCommand::BaseProgramControlCommand(
	const CmdArgParser &p,ServerConnection *c,const QByteArray &cmdPrefix,const QString &clientCommand)
	:IClientCommand(p,c)
{
	mCmdPrefix=cmdPrefix;
	mClientCmd=clientCommand;
}

bool BaseProgramControlCommand::evalCommand()
{
	if(parser.args.count()<1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",mClientCmd);
		return false;
	}
	QByteArray subCommand=parser.args[0].toUtf8();
	if(subCommand=="list")
		return writeCommandToServer(mCmdPrefix+"_list");
	else if(subCommand=="update")
	{
		if(parser.args.count()!=3)
		{
			StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
			ShowHelp::showHelp("",mClientCmd);
			return false;
		}
		QByteArray prgId=parser.args[1].toUtf8();
		QByteArray data;
		QFile file(parser.args[2]);
		if(!file.open(QIODevice::ReadOnly))
		{
			StdQFile::inst().stderrDebug()<<"can't open program's file: "+parser.args[2];
			return false;
		}
		data=file.readAll();
		file.close();
		return writeCommandToServer(mCmdPrefix+"_update",QByteArrayList()<<prgId<<data);
	}
	else if(subCommand=="rename")
	{
		if(parser.args.count()!=3)
		{
			StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
			ShowHelp::showHelp("",mClientCmd);
			return false;
		}
		return writeCommandToServer(mCmdPrefix+"_rename",
			QByteArrayList()<<parser.args[1].toUtf8()<<parser.args[2].toUtf8());
	}
	else if(subCommand!="start"&&subCommand!="stop"&&subCommand!="restart"&&
		subCommand!="get"&&subCommand!="remove")
	{
		StdQFile::inst().stderrDebug()<<"Invalid subcommand\n";
		ShowHelp::showHelp("",mClientCmd);
		return false;
	}
	if(parser.args.count()!=2)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",mClientCmd);
		return false;
	}
	QByteArray jsScriptName=parser.args[1].toUtf8();
	return writeCommandToServer(mCmdPrefix+"_"+subCommand,QByteArrayList()<<jsScriptName);
}

bool BaseProgramControlCommand::onOk(const QByteArrayList &args)
{
	if(parser.args[0]=="get"&&args.count()>0)
		StdQFile::inst().stdoutDebug()<<QString::fromUtf8(args[0]);
	return true;
}

bool BaseProgramControlCommand::onCmdData(const QByteArrayList &args)
{
	if(parser.args[0]=="list")
	{
		if(args.count()!=3)return false;
			StdQFile::inst().stdoutDebug()<<"program: "<<QString::fromUtf8(args[1])<<" ("<<args[0]<<"); status: "<<
				(args[3]=="1"?"working":"stopped");
	}
	return true;
}
