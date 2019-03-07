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

#include "ListSensorsCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include "wliot/devices/SensorDef.h"
#include <QCoreApplication>
#include <QDebug>

ListSensorsCommand::ListSensorsCommand(const CmdArgParser &p, IotServerConnection *c)
	:IClientCommand(p,c)
{
}

bool ListSensorsCommand::evalCommand()
{
	if(parser.args.count()!=1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::listSensorsCommand);
		return false;
	}
	return writeCommandToServer(IClientCommand::listSensorsCommand,stringListToByteArrayList(parser.args));
}

bool ListSensorsCommand::onOk(const QByteArrayList &args)
{
	if(args.count()!=1)return false;
	QList<SensorDef> sensors;
	if(!SensorDef::parseXmlDescription(args[0],sensors))
	{
		StdQFile::inst().stderrDebug()<<"error: can't parse sensors xml\n";
		return false;
	}
	QDebug d=StdQFile::inst().stdoutDebug();
	for(SensorDef &s:sensors)
	{
		if(forCompletion())
		{
			d<<s.name<<"\n";
		}
		else
		{
			d<<"Sensor:";
			d<<"\n\tName:"<<s.name;
			d<<"\n\tType:"<<s.type.toString();
			d<<"\n\tAttributes:";
			for(auto i=s.attributes.begin();i!=s.attributes.end();++i)
				d<<"\n\t\t"<<i.key()<<": "<<i.value();
			d<<"\n";
		}
	}
	return true;
}
