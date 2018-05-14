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
#include "Commands/DefaultCommand.h"
#include "Commands/ListTtyCommand.h"
#include "Commands/IdentifyTtyCommand.h"
#include "Commands/ExecCommandCommand.h"
#include "Commands/ListStoragesCommand.h"
#include "Commands/AddStorageCommand.h"
#include "Commands/AddStorageManualCommand.h"
#include "Commands/ListSensorsCommand.h"
#include "Commands/ListIdentifiedCommand.h"
#include "Commands/JSControlCommand.h"
#include "Commands/RegisterVirtualDeviceCommand.h"
#include "Commands/IdentifyTcpCommand.h"
#include "Commands/ListCommandsCommand.h"
#include "Commands/HelperCommand.h"
#include "StdQFile.h"
#include "ARpcBase/ARpcServerConfig.h"
#include <QCoreApplication>
#include <QDebug>

//has help
const QByteArray IClientCommand::addStorageCommand="add_storage";
const QByteArray IClientCommand::addStorageManualCommand="add_storage_manual";
const QByteArray IClientCommand::bindSensorCommand="bind_sensor";
const QByteArray IClientCommand::devicesConfigCommand="devices_config";
const QByteArray IClientCommand::execCommandCommand="exec_command";
const QByteArray IClientCommand::getSamplesCommand="get_samples";
const QByteArray IClientCommand::getSamplesCountCommand="get_samples_count";
const QByteArray IClientCommand::identifyTcpCommand="identify_tcp";
const QByteArray IClientCommand::identifyTtyCommand="identify_tty";
const QByteArray IClientCommand::jsProgramCommand="js_program";
const QByteArray IClientCommand::listCommandsCommand="list_commands";
const QByteArray IClientCommand::listIdentifiedCommand="list_identified";
const QByteArray IClientCommand::listSensorsCommand="list_sensors";
const QByteArray IClientCommand::listStoragesCommand="list_storages";
const QByteArray IClientCommand::listTtyCommand="list_tty";
const QByteArray IClientCommand::registerVirtualDeviceCommand="register_virtual_device";
const QByteArray IClientCommand::removeStorageCommand="remove_storage";
const QByteArray IClientCommand::sessionGetAttrCommand="session_get_attr";
const QByteArray IClientCommand::sessionGetWriteIdCommand="session_get_write_id";
const QByteArray IClientCommand::sessionListAttrsCommand="session_list_attrs";
const QByteArray IClientCommand::sessionListCommand="session_list";
const QByteArray IClientCommand::sessionSetAttrCommand="session_set_attr";
const QByteArray IClientCommand::sessionStartCommand="session_start";
const QByteArray IClientCommand::sessionStopCommand="session_stop";
const QByteArray IClientCommand::vdevMeasCommand="vdev_meas";

//don't has help
const QByteArray IClientCommand::subscribeCommand="subscribe";
const QByteArray IClientCommand::unsubscribeCommand="unsubscribe";

//special
static const QByteArray helperCommand="helper";

//TODO !!! специальная обработка для команды vdev_meas, чтобы вставлять даты,
	//и/или написать в справке, как это сделать с помощью средств bash

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
	else if(cmdName==identifyTcpCommand)
		return new IdentifyTcpCommand(p,d);
	else if(cmdName==execCommandCommand)
		return new ExecCommandCommand(p,d);
	else if(cmdName==listStoragesCommand)
		return new ListStoragesCommand(p,d);
	else if(cmdName==listSensorsCommand)
		return new ListSensorsCommand(p,d);
	else if(cmdName==addStorageCommand)
		return new AddStorageCommand(p,d);
	else if(cmdName==addStorageManualCommand)
		return new AddStorageManualCommand(p,d);
	else if(cmdName==removeStorageCommand)
		return new DefaultCommand(p,d,IClientCommand::removeStorageCommand,2);
	else if(cmdName==bindSensorCommand)
		return new DefaultCommand(p,d,IClientCommand::bindSensorCommand,3);
	else if(cmdName==listIdentifiedCommand)
		return new ListIdentifiedCommand(p,d);
	else if(cmdName==devicesConfigCommand)
		return new DefaultCommand(p,d,IClientCommand::devicesConfigCommand,1);
	else if(cmdName==jsProgramCommand)
		return new JSControlCommand(p,d);
	else if(cmdName==IClientCommand::getSamplesCountCommand)
		return new DefaultCommand(p,d,IClientCommand::getSamplesCountCommand,2);
	else if(cmdName==IClientCommand::getSamplesCommand)
		return new DefaultCommand(p,d,IClientCommand::getSamplesCommand,4);
	else if(cmdName==IClientCommand::registerVirtualDeviceCommand)
		return new RegisterVirtualDeviceCommand(p,d);
	else if(cmdName==IClientCommand::vdevMeasCommand)
		return new DefaultCommand(p,d,IClientCommand::vdevMeasCommand,3);
	else if(cmdName==IClientCommand::subscribeCommand)
		return new DefaultCommand(p,d,IClientCommand::subscribeCommand,2);
	else if(cmdName==IClientCommand::unsubscribeCommand)
		return new DefaultCommand(p,d,IClientCommand::unsubscribeCommand,2);
	else if(cmdName==IClientCommand::listCommandsCommand)
		return new ListCommandsCommand(p,d);
	else if(cmdName==sessionListCommand)
		return new DefaultCommand(p,d,IClientCommand::sessionListCommand,2);
	else if(cmdName==sessionListAttrsCommand)
		return new DefaultCommand(p,d,IClientCommand::sessionListAttrsCommand,3);
	else if(cmdName==sessionGetAttrCommand)
		return new DefaultCommand(p,d,IClientCommand::sessionGetAttrCommand,4);
	else if(cmdName==sessionSetAttrCommand)
		return new DefaultCommand(p,d,IClientCommand::sessionSetAttrCommand,4);
	else if(cmdName==sessionGetWriteIdCommand)
		return new DefaultCommand(p,d,IClientCommand::sessionGetWriteIdCommand,2);
	else if(cmdName==sessionStartCommand)
		return new DefaultCommand(p,d,IClientCommand::sessionStartCommand,3);
	else if(cmdName==sessionStopCommand)
		return new DefaultCommand(p,d,IClientCommand::sessionStopCommand,2);
	else if(cmdName==helperCommand)
		return new HelperCommand(p,d);
	else
	{
		qDebug()<<"Unknown command: "<<cmdName<<"; use "<<qApp->arguments()[0]<<" --help to see help message";
		return 0;
	}
}

bool IClientCommand::needToBeSilent(CmdArgParser &p)
{
	if(!p.getArgs().isEmpty())
	{
		QString cmdName=p.getArgs()[0];
		if(cmdName=="helper")return true;
	}
	return false;
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
			qApp->exit(exitErrorCode);
		}
		else qApp->exit(0);
	}
	else if(m.title==ARpcConfig::funcAnswerErrMsg)
	{
		onErr(m.args);
		qApp->exit(exitErrorCode);
	}
}

bool IClientCommand::onOk(const QByteArrayList &args)
{
	if(!args.isEmpty())StdQFile::inst().stdoutDebug()<<args.join("|")<<"\n";
	return true;
}

void IClientCommand::onErr(const QByteArrayList &args)
{
	if(!args.isEmpty())StdQFile::inst().stderrDebug()<<args.join("|")<<"\n";
}

bool IClientCommand::onCmdData(const QByteArrayList &args)
{
	if(!args.isEmpty())StdQFile::inst().stdoutDebug()<<"data: "<<args.join("|")<<"\n";
	return true;
}

QByteArrayList IClientCommand::stringListToByteArrayList(const QStringList &list)
{
	QByteArrayList retVal;
	for(const QString &s:list)
		retVal.append(s.toUtf8());
	return std::move(retVal);
}
