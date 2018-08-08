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

#include "ExecCommandCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

quint64 ExecCommandCommand::callIdStatic=0;

ExecCommandCommand::ExecCommandCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool ExecCommandCommand::evalCommand()
{
	if(parser.getArgs().count()<2)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::execCommandCommand);
		return false;
	}
	QByteArrayList args=stringListToByteArrayList(parser.getArgs());
	callId=QByteArray::number(callIdStatic++);
	args.prepend(callId);
	return dev->writeMsg(IClientCommand::execCommandCommand,args);
}

bool ExecCommandCommand::onOk(const QByteArrayList &args)
{
	if(args.count()>1&&args[0]==callId)
	{
		StdQFile::inst().stdoutDebug()<<"Command result: "<<args.mid(1).join(ARpcConfig::argDelim)<<"\n";
		return true;
	}
	else if(args.count()==1&&args[0]==callId)return true;
	else
	{
		StdQFile::inst().stderrDebug()<<"Invalid answer from server";
		return false;
	}
}
