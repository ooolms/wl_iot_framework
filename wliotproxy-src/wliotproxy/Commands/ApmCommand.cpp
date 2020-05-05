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

#include "ApmCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>
#include <termios.h>
#include <unistd.h>
#include <iostream>

bool setStdinEchoMode(bool en);

static QByteArray readOldPassword()
{
	setStdinEchoMode(false);
	std::cout<<"Old password: ";
	std::string s;
	std::cin>>s;
	std::cout<<"\n";
	setStdinEchoMode(true);
	return QByteArray::fromStdString(s);
}

static QByteArray readNewPassword()
{
	setStdinEchoMode(false);
	std::cout<<"Create new password: ";
	std::string s1,s2;
	std::cin>>s1;
	std::cout<<"\nConfirm new password:";
	std::cin>>s2;
	std::cout<<"\n";
	setStdinEchoMode(true);
	if(s1!=s2)
	{
		std::cout<<"don't match";
		return QByteArray();
	}
	return QByteArray::fromStdString(s1);
}

ApmCommand::ApmCommand(const CmdArgParser &p,IotServerConnection *c)
	:IClientCommand(p,c)
{
}

bool ApmCommand::evalCommand()
{
	QByteArrayList args=stringListToByteArrayList(parser.args);
	if(args.count()<2)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::apmCommand);
		return false;
	}
	if(args[0]=="user")
	{
		if((args[1]=="passwd"&&args.count()>2)||(args[1]=="chpass"&&args.count()>3)||
			(args[1]=="create"&&args.count()>3))
		{
			StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
			ShowHelp::showHelp("",IClientCommand::apmCommand);
			return false;
		}
		if(args[1]=="passwd")
		{
			QByteArray oldPassword=readOldPassword();
			if(oldPassword.isEmpty())
			{
				StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
				ShowHelp::showHelp("",IClientCommand::apmCommand);
				return false;
			}
			args.append(oldPassword);
		}
		if(args[1]=="chpass"||args[1]=="passwd"||args[1]=="create")
		{
			QByteArray password=readNewPassword();
			if(password.isEmpty())
			{
				StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
				ShowHelp::showHelp("",IClientCommand::apmCommand);
				return false;
			}
			args.append(password);
		}
	}
	return writeCommandToServer(IClientCommand::apmCommand.toUtf8(),args);
}
