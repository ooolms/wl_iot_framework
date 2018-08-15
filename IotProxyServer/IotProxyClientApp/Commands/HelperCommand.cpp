/*******************************************
Copyright 2018 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "HelperCommand.h"
#include <QCoreApplication>
#include "../StdQFile.h"
#include "../ShowHelp.h"
#include <QDebug>

HelperCommand::HelperCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool HelperCommand::evalCommand()
{
	if(parser.getArgs().count()<1)return true;// !!! no need to fail
	QString subCmd=parser.getArgs()[0];
	if(subCmd=="list_identified")
	{
		onCmdDataFunc=[this](const QByteArrayList &args)
		{
			if(args.count()<2)return;
			wordList.append(args[0]);
			wordList.append(args[1]);
		};
		dev->writeMsg(IClientCommand::listIdentifiedCommand);
	}
	else if(subCmd=="list_storages_devs")
	{
		onCmdDataFunc=[this](const QByteArrayList &args)
		{
			if(args.count()<2)return;
			wordList.append(args[0]);
			wordList.append(args[1]);
		};
		dev->writeMsg(IClientCommand::listStoragesCommand);
	}
	else if(subCmd=="list_storages_sensors")
	{
		if(parser.getArgs().count()<2)return true;
		QString devNameOrId=parser.getArgs()[1];
		onCmdDataFunc=[this,devNameOrId](const QByteArrayList &args)
		{
			if(args.count()<3)return;
			if(args[0]==devNameOrId||args[1]==devNameOrId)
				wordList.append(args[2]);
		};
		dev->writeMsg(IClientCommand::listStoragesCommand);
	}
	else if(subCmd=="list_external_services")
	{
		//TODO services from server
		QMetaObject::invokeMethod(this,"done",Qt::QueuedConnection);
	}
	return true;
}

void HelperCommand::done()
{
	wordList.append("alterozoom");
	wordList.append("iotkit-agent");
	onOk(QByteArrayList());
	qApp->exit(0);
}

bool HelperCommand::onCmdData(const QByteArrayList &args)
{
	onCmdDataFunc(args);
	return true;
}

bool HelperCommand::onOk(const QByteArrayList &args)
{
	Q_UNUSED(args)
	if(!wordList.isEmpty())
		StdQFile::inst().stdoutDebug()<<wordList.join(" ")<<"\n";
	return true;
}
