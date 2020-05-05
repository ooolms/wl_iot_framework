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

#include "AlterozoomAuthCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include "ExternServices/AlterozoomAuthentificationStorage.h"
#include <QDebug>
#include <termios.h>
#include <unistd.h>
#include <iostream>

bool setStdinEchoMode(bool en);

static QByteArray readPassword()
{
	setStdinEchoMode(false);
	std::cout<<"Password: ";
	std::string s;
	std::cin>>s;
	std::cout<<"\n";
	setStdinEchoMode(true);
	return QByteArray::fromStdString(s);
}

AlterozoomAuthCommand::AlterozoomAuthCommand(const CmdArgParser &p,IotServerConnection *c)
	:IClientCommand(p,c)
{
	AlterozoomAuthentificationStorage::readConfig("/var/lib/wliotproxyd/alterozoom_auth.xml");
}

bool AlterozoomAuthCommand::evalCommand()
{
	QByteArrayList args=stringListToByteArrayList(parser.args);
	if(parser.args.count()<1||parser.args.count()>2)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::alterozoomAuthCommand);
		return false;
	}
	QByteArray host;
	QByteArray email=parser.args[0].toUtf8();
	if(parser.args.count()==2)
		host=parser.args[1].toUtf8();
	else host=AlterozoomAuthentificationStorage::getDefaultHost();
	if(host.isEmpty()||email.isEmpty())
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::alterozoomAuthCommand);
		return false;
	}
	QByteArray password=readPassword();
	if(password.isEmpty())
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::alterozoomAuthCommand);
		return false;
	}
	return writeCommandToServer(IClientCommand::alterozoomAuthCommand.toUtf8(),QByteArrayList()<<host<<email<<password);
}
