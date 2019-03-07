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

#include "AddStorageCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

AddStorageCommand::AddStorageCommand(const CmdArgParser &p,IotServerConnection *c)
	:IClientCommand(p,c)
{
}

bool AddStorageCommand::evalCommand()
{
	if(parser.args.count()!=4)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::addStorageCommand);
		return false;
	}
	int valuesCount=1;
	ISensorStorage::StoreMode mode=ISensorStorage::storeModeFromString(parser.args[2].toUtf8());
	if(mode==ISensorStorage::BAD_MODE)
	{
		StdQFile::inst().stderrDebug()<<"Invalid argument: "<<parser.args[2]<<"\n";
		ShowHelp::showHelp("",IClientCommand::addStorageCommand);
		return false;
	}
	else if((mode==ISensorStorage::LAST_N_VALUES||mode==ISensorStorage::LAST_N_VALUES_IN_MEMORY)&&
		parser.vars.contains("N"))
	{
		bool ok=false;
		valuesCount=parser.getVarSingle("N").toInt(&ok);
		if(!ok||valuesCount==0)
			valuesCount=1;
	}
	ISensorStorage::TimestampRule tsRule;
	if(!ISensorStorage::timestampRuleFromString(parser.args[3].toUtf8(),tsRule))
	{
		StdQFile::inst().stderrDebug()<<"Invalid argument: "<<parser.args[3]<<"\n";
		ShowHelp::showHelp("",IClientCommand::addStorageCommand);
		return false;
	}
	if(mode==ISensorStorage::LAST_N_VALUES||mode==ISensorStorage::LAST_N_VALUES_IN_MEMORY)
		return writeCommandToServer(IClientCommand::addStorageCommand,
			QByteArrayList()<<stringListToByteArrayList(parser.args)<<QByteArray::number(valuesCount));
	else
		return writeCommandToServer(IClientCommand::addStorageCommand,stringListToByteArrayList(parser.args));
}
