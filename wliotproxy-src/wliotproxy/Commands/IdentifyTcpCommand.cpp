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

#include "IdentifyTcpCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

using namespace WLIOTClient;

IdentifyTcpCommand::IdentifyTcpCommand(const CmdArgParser &p, ServerConnection *c)
	:IClientCommand(p,c)
{
}

bool IdentifyTcpCommand::evalCommand()
{
	if(parser.args.count()!=1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::identifyTcpCommand);
		return false;
	}
	return writeCommandToServer(IClientCommand::identifyTcpCommand.toUtf8(),stringListToByteArrayList(parser.args));
}

bool IdentifyTcpCommand::onOk(const QByteArrayList &args)
{
	if(args.count()==2)
	{
		StdQFile::inst().stdoutDebug()<<"Device identified: id="<<args[0]<<" name="<<args[1]<<"\n";
		return true;
	}
	return false;
}
