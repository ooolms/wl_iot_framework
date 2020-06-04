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

#include "VDILControlCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include "VDIL/core/TimerBlock.h"
#include <QDebug>

using namespace WLIOTVDIL;

VDILControlCommand::VDILControlCommand(const CmdArgParser &p,WLIOTClient::ServerConnection *c)
	:BaseProgramControlCommand(p,c,"vdil",IClientCommand::vdilProgramCommand)
{
}

bool VDILControlCommand::evalCommand()
{
	if(parser.args.count()<1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::vdilProgramCommand);
		return false;
	}
	QByteArray subCommand=parser.args[0].toUtf8();
	if(subCommand=="list_config_options"||subCommand=="list_timers"||
		subCommand=="set_config_option"||subCommand=="set_timer")
		return writeCommandToServer("vdil_"+subCommand,stringListToByteArrayList(parser.args));
	else return BaseProgramControlCommand::evalCommand();
}

bool VDILControlCommand::onCmdData(const QByteArrayList &args)
{
	if(parser.args[0]=="list_config_options")
	{
		if(args.count()<6)
			return false;
		StdQFile::inst().stdoutDebug()<<args[1]<<":"<<args[2]<<"; "<<args[3]<<":"<<args[4]<<"d:"<<args.mid(5).join("|");
		return true;
	}
	else if(parser.args[0]=="list_timers")
	{
		if(args.count()<5)
			return false;
		TimerBlock::TimerConfig cfg;
		cfg.startTime=QDateTime::fromMSecsSinceEpoch(args[2].toLongLong()*1000);
		cfg.policy=TimerBlock::policyFromStr(args[3]);
		cfg.repeatInterval=args[4].toLongLong();
		StdQFile::inst().stdoutDebug()<<args[1]<<"; "<<TimerBlock::configString(cfg);
		return true;
	}
	else return BaseProgramControlCommand::onCmdData(args);
}
