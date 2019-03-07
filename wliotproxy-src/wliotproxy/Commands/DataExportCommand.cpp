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

#include "DataExportCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

DataExportCommand::DataExportCommand(const CmdArgParser &p, IotServerConnection *c)
	:IClientCommand(p,c)
{
}

bool DataExportCommand::evalCommand()
{
	if(parser.args.count()<3)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::dataExportCommand);
		return false;
	}
	subCommand=parser.args[0].toUtf8();
	QByteArray devIdOrName=parser.args[1].toUtf8();
	QByteArray sensorName=parser.args[2].toUtf8();
	if(subCommand=="list")
		return writeCommandToServer("storage_get_data_export_list",QByteArrayList()<<devIdOrName<<sensorName);
	else if(subCommand!="get"&&subCommand!="add"&&subCommand!="remove")
	{
		StdQFile::inst().stderrDebug()<<"Invalid subcommand\n";
		ShowHelp::showHelp("",IClientCommand::dataExportCommand);
		return false;
	}
	if(parser.args.count()<4)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::dataExportCommand);
		return false;
	}
	QByteArray serviceType=parser.args[3].toUtf8();
	if(subCommand=="get")
		return writeCommandToServer("storage_get_data_export",QByteArrayList()<<devIdOrName<<sensorName<<serviceType);
	else if(subCommand=="remove")
		return writeCommandToServer("storage_add_data_export",QByteArrayList()<<devIdOrName<<sensorName<<serviceType);
	else //add
		return writeCommandToServer("storage_add_data_export",QByteArrayList()<<devIdOrName<<sensorName<<serviceType<<
			stringListToByteArrayList(parser.args.mid(4)));
}

bool DataExportCommand::onOk(const QByteArrayList &args)
{
	if(subCommand=="list")
	{
		StdQFile::inst().stdoutDebug()<<"External services configured for data export:\n";
		StdQFile::inst().stdoutDebug()<<args.join(", ")<<"\n";
		return true;
	}
	else if(subCommand=="get")
	{
		StdQFile::inst().stdoutDebug()<<"External service configuration:\n";
		for(QByteArray a:args)
		{
			a.replace(":",": ");
			StdQFile::inst().stdoutDebug()<<"\t"<<a<<"\n";
		}
		return true;
	}
	else return IClientCommand::onOk(args);
}
