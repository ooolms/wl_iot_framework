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

#include "ListCommandsCommand.h"
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include "wliot/devices/ControlsDefinition.h"
#include <QDebug>
#include <functional>

using namespace WLIOT;
using namespace WLIOTClient;

ListCommandsCommand::ListCommandsCommand(const CmdArgParser &p, ServerConnection *c)
	:IClientCommand(p,c)
{
}

bool ListCommandsCommand::evalCommand()
{
	if(parser.args.count()<1)
	{
		StdQFile::inst().stderrDebug()<<"Invalid arguments\n";
		ShowHelp::showHelp("",IClientCommand::listCommandsCommand);
		return false;
	}
	return writeCommandToServer("list_controls",QByteArrayList()<<parser.args[0].toUtf8());
}

bool ListCommandsCommand::onOk(const QByteArrayList &args)
{
	if(args.count()<1)return false;
	ControlsGroup controls;
	if(!ControlsGroup::parseXmlDescription(args[0],controls))return false;
	QList<CommandControl> commands;
	std::function<void(ControlsGroup&)> lsGrp=[&controls,&commands,&lsGrp](ControlsGroup &grp)->void
	{
		for(auto &el:grp.elements)
		{
			if(el.isGroup())
				lsGrp(*el.group());
			else commands.append(*el.control());
		}
	};
	lsGrp(controls);
	QDebug out=StdQFile::inst().stdoutDebug();
	if(!forCompletion())out<<"Commands list with arguments\n";
	for(auto &c:commands)
	{
		if(forCompletion())
			out<<c.command<<" ";
		else
		{
			out<<"COMMAND: "<<c.command<<"\n";
			for(auto &p:c.params)
				out<<"\tname: "<<p.title<<"\n\tattributes: "<<p.attributes<<"\n";
		}
	}
	return true;
}
