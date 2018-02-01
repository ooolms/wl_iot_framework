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

RegisterVirtualDeviceCommand::RegisterVirtualDeviceCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool RegisterVirtualDeviceCommand::evalCommand()
{
	if(parser.getArgs().count()!=3||parser.getArgs()[0].isEmpty()||parser.getArgs()[1].isEmpty())
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::registerVirtualDeviceCommand);
		return false;
	}
	QByteArray sensorsDescr;
	QFile file(parser.getArgs()[2]);
	if(!file.open(QIODevice::ReadOnly))
	{
		StdQFile::inst().stderrDebug()<<"Can't open file with sensors description";
		return false;
	}
	sensorsDescr=file.readAll();
	file.close();
	return dev->writeMsg(IClientCommand::registerVirtualDeviceCommand,
		QByteArrayList()<<parser.getArgs()[0].toUtf8()<<parser.getArgs()[1].toUtf8()<<sensorsDescr);
}
