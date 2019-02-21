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

#include "RegisterVirtualDeviceCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

RegisterVirtualDeviceCommand::RegisterVirtualDeviceCommand(const CmdArgParser &p, IotServerConnection *c)
	:IClientCommand(p,c)
{
}

bool RegisterVirtualDeviceCommand::evalCommand()
{
	if(parser.args.count()!=3||parser.args[0].isEmpty()||parser.args[1].isEmpty())
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::registerVirtualDeviceCommand);
		return false;
	}
	QByteArray sensorsDescr;
	QFile file(parser.args[2]);
	if(!file.open(QIODevice::ReadOnly))
	{
		StdQFile::inst().stderrDebug()<<"Can't open file with sensors description";
		return false;
	}
	sensorsDescr=file.readAll();
	file.close();
	return writeCommandToServer(IClientCommand::registerVirtualDeviceCommand,
		QByteArrayList()<<parser.args[0].toUtf8()<<parser.args[1].toUtf8()<<sensorsDescr);
}
