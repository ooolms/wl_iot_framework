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

#include "IClientCommand.h"
#include "Commands/ListTtyCommand.h"
#include "Commands/IdentifyTtyCommand.h"
#include "Commands/ExecCommandCommand.h"
#include "Commands/ListStoragesCommand.h"
#include "Commands/AddSensorCommand.h"
#include "Commands/ListSensorsCommand.h"
#include "Commands/RemoveSensorCommand.h"
#include "Commands/BindSensorCommand.h"
#include "Commands/ListIdentifiedCommand.h"
#include "Commands/DevicesConfigCommand.h"
#include "Commands/JSControlCommand.h"
#include "StdQFile.h"
#include "ARpcBase/ARpcServerConfig.h"
#include <QCoreApplication>
#include <QDebug>

const QString IClientCommand::listTtyCommand=QString("list_tty");
const QString IClientCommand::identifyTtyCommand=QString("identify_tty");
const QString IClientCommand::listSensorsCommand=QString("list_sensors");
const QString IClientCommand::execCommandCommand=QString("exec_command");
const QString IClientCommand::listStoragesCommand=QString("list_storages");
const QString IClientCommand::addSensorCommand=QString("add_sensor");
const QString IClientCommand::removeSensorCommand=QString("remove_sensor");
const QString IClientCommand::bindSensorCommand=QString("bind_sensor");
const QString IClientCommand::listIdentifiedCommand=QString("list_identified");
const QString IClientCommand::devicesConfigCommand=QString("devices_config");
const QString IClientCommand::jsProgramCommand=QString("js_program");

IClientCommand::IClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:parser(p)
{
	dev=d;
	exitErrorCode=1;
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IClientCommand::processMessage);
}

IClientCommand* IClientCommand::mkCommand(CmdArgParser &p,ARpcOutsideDevice *d)
{
	if(p.getArgs().isEmpty())
	{
		qDebug()<<"No command specified";
		return 0;
	}
	QString cmdName=p.getArgs()[0];
	p.getArgsToChange().removeAt(0);
	if(cmdName==listTtyCommand)
		return new ListTtyCommand(p,d);
	else if(cmdName==identifyTtyCommand)
		return new IdentifyTtyCommand(p,d);
	else if(cmdName==execCommandCommand)
		return new ExecCommandCommand(p,d);
	else if(cmdName==listStoragesCommand)
		return new ListStoragesCommand(p,d);
	else if(cmdName==listSensorsCommand)
		return new ListSensorsCommand(p,d);
	else if(cmdName==addSensorCommand)
		return new AddSensorCommand(p,d);
	else if(cmdName==removeSensorCommand)
		return new RemoveSensorCommand(p,d);
	else if(cmdName==bindSensorCommand)
		return new BindSensorCommand(p,d);
	else if(cmdName==listIdentifiedCommand)
		return new ListIdentifiedCommand(p,d);
	else if(cmdName==devicesConfigCommand)
		return new DevicesConfigCommand(p,d);
	else if(cmdName==jsProgramCommand)
		return new JSControlCommand(p,d);
	else
	{
		qDebug()<<"Unknown command: "<<cmdName<<"; use "<<qApp->arguments()[0]<<" --help to see help message";
		return 0;
	}
}

void IClientCommand::setExitErrorCode(int code)
{
	exitErrorCode=code;
}

void IClientCommand::processMessage(const ARpcMessage &m)
{
	if(m.title==ARpcServerConfig::srvCmdDataMsg)
	{
		if(!onCmdData(m.args))
		{
			StdQFile::inst().stdoutDebug()<<"Error\n";
			qApp->exit(exitErrorCode);
		}
	}
	else if(m.title==ARpcConfig::infoMsg)
	{
		StdQFile::inst().stdoutDebug()<<m.args.join("|")<<"\n";
	}
	else if(m.title==ARpcConfig::funcAnswerOkMsg)
	{
		if(!onOk(m.args))
		{
			StdQFile::inst().stdoutDebug()<<"Error\n";
			qApp->exit(exitErrorCode);
		}
		else
		{
			StdQFile::inst().stdoutDebug()<<"Ok\n";
			qApp->exit(0);
		}
	}
	else if(m.title==ARpcConfig::funcAnswerErrMsg)
	{
		onErr(m.args);
		StdQFile::inst().stdoutDebug()<<"Daemon error: "<<m.args.join("|")<<"\n";
		qApp->exit(exitErrorCode);
	}
}

bool IClientCommand::onOk(const QStringList &args)
{
	Q_UNUSED(args)
	return true;
}

void IClientCommand::onErr(const QStringList &args)
{
	Q_UNUSED(args)
}

bool IClientCommand::onCmdData(const QStringList &args)
{
	Q_UNUSED(args)
	return true;
}
