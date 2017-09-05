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
#include "ARpcBase/ARpcSensor.h"
#include <QCoreApplication>
#include <QDebug>

ListSensorsCommand::ListSensorsCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool ListSensorsCommand::evalCommand()
{
	if(parser.getArgs().count()!=1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("","list_sensors");
		return false;
	}
	return dev->writeMsg(IClientCommand::listSensorsCommand,parser.getArgs());
}

bool ListSensorsCommand::onOk(const QStringList &args)
{
	if(args.count()!=1)return false;
	QList<ARpcSensor> sensors;
	if(!ARpcSensor::parseXmlDescription(args[0],sensors))
	{
		StdQFile::inst().stderrDebug()<<"error: can't parse sensors xml\n";
		return false;
	}
	QDebug d=StdQFile::inst().stdoutDebug();
	for(ARpcSensor &s:sensors)
	{
		d<<"Sensor:";
		d<<"\n\tName:"<<s.name;
		d<<"\n\tType:"<<ARpcSensor::typeToString(s.type);
		d<<"\n\tConstrains:";
		for(auto i=s.constraints.begin();i!=s.constraints.end();++i)
			d<<"\n\t\t"<<i.key()<<": "<<i.value().toString();
		d<<"\n";
	}
	return true;
}
