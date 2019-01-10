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

#include "AddStorageManualCommand.h"
#include "ARpcStorages/ARpcBaseFSSensorStorage.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

AddStorageManualCommand::AddStorageManualCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool AddStorageManualCommand::evalCommand()
{
	if(parser.args.count()!=6)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::addStorageManualCommand);
		return false;
	}
	int valuesCount=1;
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(parser.args[4].toUtf8());
	if(mode==ARpcISensorStorage::BAD_MODE)
	{
		StdQFile::inst().stderrDebug()<<"Invalid argument: "<<parser.args[4]<<"\n";
		ShowHelp::showHelp("",IClientCommand::addStorageManualCommand);
		return false;
	}
	else if((mode==ARpcISensorStorage::LAST_N_VALUES||mode==ARpcISensorStorage::LAST_N_VALUES_IN_MEMORY)&&
		parser.vars.contains("N"))
	{
		bool ok=false;
		valuesCount=parser.getVarSingle("N").toInt(&ok);
		if(!ok||valuesCount==0)
			valuesCount=1;
	}
	ARpcISensorStorage::TimestampRule tsRule;
	if(!ARpcISensorStorage::timestampRuleFromString(parser.args[5].toUtf8(),tsRule))
	{
		StdQFile::inst().stderrDebug()<<"Invalid argument: "<<parser.args[5]<<"\n";
		ShowHelp::showHelp("",IClientCommand::addStorageManualCommand);
		return false;
	}
	if(mode==ARpcISensorStorage::LAST_N_VALUES||mode==ARpcISensorStorage::LAST_N_VALUES_IN_MEMORY)
		return writeCommandToServer(IClientCommand::addStorageManualCommand,
			QByteArrayList()<<stringListToByteArrayList(parser.args)<<QByteArray::number(valuesCount));
	else
		return writeCommandToServer(IClientCommand::addStorageManualCommand,stringListToByteArrayList(parser.args));
}
