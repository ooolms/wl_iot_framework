#include "IClientCommand.h"
#include "Commands/ListTtyCommand.h"
#include "Commands/IdentifyTtyCommand.h"
#include "Commands/ExecCommandCommand.h"
#include "Commands/ListStoragesCommand.h"
#include "StdQFile.h"
#include <QCoreApplication>
#include <QDebug>

const QString IClientCommand::listTtyCommand=QString("list_tty");
const QString IClientCommand::identifyTtyCommand=QString("identify_tty");
const QString IClientCommand::listSensorsCommand=QString("list_sensors");
const QString IClientCommand::execCommandCommand=QString("exec_command");
const QString IClientCommand::listStoragesCommand=QString("list_storages");
const QString IClientCommand::addSensorCommand=QString("add_sensor");
const QString IClientCommand::removeSensorCommand=QString("remove_sensor");

IClientCommand::IClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:parser(p)
{
	dev=d;
	exitErrorCode=1;
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
	if(m.title==ARpcConfig::funcAnswerOkMsg)
	{
		QDebug(StdQFile::inst().stdout())<<"Ok";
		qApp->exit(0);
	}
	else if(m.title==ARpcConfig::funcAnswerErrMsg)
	{
		QDebug(StdQFile::inst().stdout())<<"Error: "<<m.args.join("|");
		qApp->exit(exitErrorCode);
	}
}
