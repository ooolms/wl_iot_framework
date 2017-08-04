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

ExecCommandCommand::ExecCommandCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
	connect(d,&ARpcOutsideDevice::rawMessage,this,&ExecCommandCommand::onRawMessage);
}

bool ExecCommandCommand::evalCommand()
{
	bool sync=!parser.hasKey("noreturn");
	if(parser.getArgs().count()<2)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("","exec_command");
		return false;
	}
	QStringList args=parser.getArgs();
	QString devIdOrName=args[0];
	args.removeFirst();
	return dev->writeMsg(IClientCommand::execCommandCommand,QStringList()<<devIdOrName<<(sync?"1":"0")<<args);
}

void ExecCommandCommand::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::srvCmdDataMsg)
		StdQFile::inst().stdoutDebug()<<"Command result: "<<m.args.join(ARpcConfig::argDelim);
}