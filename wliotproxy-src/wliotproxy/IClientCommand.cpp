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
#include "Commands/ApmCommand.h"
#include "Commands/AlterozoomAuthCommand.h"
#include "Commands/DefaultCommand.h"
#include "Commands/ListTtyCommand.h"
#include "Commands/IdentifyTtyCommand.h"
#include "Commands/ExecCommandCommand.h"
#include "Commands/ListStoragesCommand.h"
#include "Commands/AddStorageCommand.h"
#include "Commands/ListSensorsCommand.h"
#include "Commands/GetSamplesCommand.h"
#include "Commands/ListIdentifiedCommand.h"
#include "Commands/JSControlCommand.h"
#include "Commands/IdentifyTcpCommand.h"
#include "Commands/ListCommandsCommand.h"
#include "Commands/HelperCommand.h"
#include "Commands/SessionCommand.h"
#include "Commands/DataExportCommand.h"
#include "StdQFile.h"
#include "wliot/WLIOTServerProtocolDefs.h"
#include <QCoreApplication>
#include <QDebug>

//CRIT rewrite data_export help, write storage_get_attr and storage_set_attr help
//CRIT write apm help

//has help
const QByteArray IClientCommand::addStorageCommand="add_storage";
const QByteArray IClientCommand::alterozoomAuthCommand="alterozoom_auth";
const QByteArray IClientCommand::apmCommand="apm";
const QByteArray IClientCommand::availableDataExportServicesCommand="available_data_export_services";
const QByteArray IClientCommand::dataExportCommand="data_export";
const QByteArray IClientCommand::devicesConfigCommand="devices_config";
const QByteArray IClientCommand::devNamesCommand="dev_names";
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
const QByteArray IClientCommand::removeStorageCommand="remove_storage";
const QByteArray IClientCommand::sessionCommand="session";
const QByteArray IClientCommand::setDevNameCommand="set_dev_name";
const QByteArray IClientCommand::storageGetAttrCommand="storage_get_attr";
const QByteArray IClientCommand::storageSetAttrCommand="storage_set_attr";
const QByteArray IClientCommand::vdevMeasCommand="vdev_meas";

//don't has help
const QByteArray IClientCommand::subscribeCommand="subscribe";
const QByteArray IClientCommand::unsubscribeCommand="unsubscribe";

//special
static const QByteArray helperCommand="helper";

//TODO !!! специальная обработка для команды vdev_meas, чтобы вставлять даты,
	//и/или написать в справке, как это сделать с помощью средств bash

bool IClientCommand::mForCompletion=false;

IClientCommand::IClientCommand(const CmdArgParser &p,IotServerConnection *c)
	:parser(p)
{
	conn=c;
	exitErrorCode=1;
	callId=QByteArray::number(qrand());
	connect(conn,&IotServerConnection::funcCallReplyMsg,this,&IClientCommand::processMessage);
}

IClientCommand* IClientCommand::mkCommand(CmdArgParser &p,IotServerConnection *c)
{
	mForCompletion=p.keys.contains("compl");
	if(p.args.isEmpty())
	{
		if(!mForCompletion)
			qDebug()<<"No command specified";
		return 0;
	}
	QString cmdName=p.args[0];
	p.args.removeAt(0);
	if(cmdName==addStorageCommand)
		return new AddStorageCommand(p,c);
	else if(cmdName==alterozoomAuthCommand)
		return new AlterozoomAuthCommand(p,c);
	else if(cmdName==availableDataExportServicesCommand)
		return new DefaultCommand(p,c,availableDataExportServicesCommand,0);
	else if(cmdName==apmCommand)
		return new ApmCommand(p,c);
	else if(cmdName==dataExportCommand)
		return new DataExportCommand(p,c);
	else if(cmdName==devicesConfigCommand)
		return new DefaultCommand(p,c,devicesConfigCommand,1);
	else if(cmdName==devNamesCommand)
		return new DefaultCommand(p,c,devNamesCommand,1);
	else if(cmdName==execCommandCommand)
		return new ExecCommandCommand(p,c);
	else if(cmdName==getSamplesCommand)
		return new GetSamplesCommand(p,c);
	else if(cmdName==getSamplesCountCommand)
		return new DefaultCommand(p,c,getSamplesCountCommand,2);
	else if(cmdName==identifyTcpCommand)
		return new IdentifyTcpCommand(p,c);
	else if(cmdName==identifyTtyCommand)
		return new IdentifyTtyCommand(p,c);
	else if(cmdName==jsProgramCommand)
		return new JSControlCommand(p,c);
	else if(cmdName==listCommandsCommand)
		return new ListCommandsCommand(p,c);
	else if(cmdName==listIdentifiedCommand)
		return new ListIdentifiedCommand(p,c);
	else if(cmdName==listSensorsCommand)
		return new ListSensorsCommand(p,c);
	else if(cmdName==listStoragesCommand)
		return new ListStoragesCommand(p,c);
	else if(cmdName==listTtyCommand)
		return new ListTtyCommand(p,c);
	else if(cmdName==removeStorageCommand)
		return new DefaultCommand(p,c,removeStorageCommand,2);
	else if(cmdName==sessionCommand)
		return new SessionCommand(p,c);
	else if(cmdName==setDevNameCommand)
		return new DefaultCommand(p,c,setDevNameCommand,2);
	else if(cmdName==storageGetAttrCommand)
		return new DefaultCommand(p,c,storageGetAttrCommand,3);
	else if(cmdName==storageSetAttrCommand)
		return new DefaultCommand(p,c,storageSetAttrCommand,4);
	else if(cmdName==vdevMeasCommand)
		return new DefaultCommand(p,c,vdevMeasCommand,3);

	else if(cmdName==subscribeCommand)
		return new DefaultCommand(p,c,subscribeCommand,2);
	else if(cmdName==unsubscribeCommand)
		return new DefaultCommand(p,c,unsubscribeCommand,2);
	else if(cmdName==helperCommand)
		return new HelperCommand(p,c);
	else
	{
		if(!mForCompletion)
			qDebug()<<"Unknown command: "<<cmdName<<"; use "<<qApp->arguments()[0]<<" --help to see help message";
		return 0;
	}
}

bool IClientCommand::forCompletion()
{
	return mForCompletion;
}

void IClientCommand::setExitErrorCode(int code)
{
	exitErrorCode=code;
}

void IClientCommand::processMessage(const Message &m)
{
	if(m.title==WLIOTServerProtocolDefs::srvCmdDataMsg)
	{
		if(m.args.isEmpty()||m.args[0]!=callId)return;
		if(!onCmdData(m.args.mid(1)))
		{
			qApp->exit(exitErrorCode);
		}
	}
	else if(m.title==WLIOTProtocolDefs::infoMsg)
	{
		if(!mForCompletion)
			StdQFile::inst().stdoutDebug()<<m.args.join("|")<<"\n";
	}
	else if(m.title==WLIOTProtocolDefs::funcAnswerOkMsg)
	{
		if(m.args.isEmpty()||m.args[0]!=callId)return;
		if(!onOk(m.args.mid(1)))
		{
			qApp->exit(exitErrorCode);
		}
		else qApp->exit(0);
	}
	else if(m.title==WLIOTProtocolDefs::funcAnswerErrMsg)
	{
		if(m.args.isEmpty()||m.args[0]!=callId)return;
		onErr(m.args.mid(1));
		qApp->exit(exitErrorCode);
	}
}

bool IClientCommand::onOk(const QByteArrayList &args)
{
	if(!args.isEmpty())
	{
		if(mForCompletion)
			StdQFile::inst().stdoutDebug()<<args.join(" ");
		else StdQFile::inst().stdoutDebug()<<args.join("|")<<"\n";
	}
	return true;
}

void IClientCommand::onErr(const QByteArrayList &args)
{
	if(!args.isEmpty()&&!mForCompletion)
		StdQFile::inst().stderrDebug()<<args.join("|")<<"\n";
}

bool IClientCommand::onCmdData(const QByteArrayList &args)
{
	if(!args.isEmpty())
	{
		if(mForCompletion)
			StdQFile::inst().stdoutDebug()<<args.join(" ");
		else StdQFile::inst().stdoutDebug()<<"data: "<<args.join("|")<<"\n";
	}
	return true;
}

QByteArrayList IClientCommand::stringListToByteArrayList(const QStringList &list)
{
	QByteArrayList retVal;
	for(const QString &s:list)
		retVal.append(s.toUtf8());
	return std::move(retVal);
}

bool IClientCommand::writeCommandToServer(const QByteArray &cmd,const QByteArrayList &args)
{
	return conn->writeMsg(Message(cmd,QByteArrayList()<<callId<<args));
}
