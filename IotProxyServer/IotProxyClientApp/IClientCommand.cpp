#include "IClientCommand.h"
#include "Commands/ListTtyCommand.h"
#include "Commands/IdentifyTtyCommand.h"
#include <QCoreApplication>
#include <QDebug>

const QString IClientCommand::listTtyCommand=QString("list_tty");
const QString IClientCommand::listTtyCommand=QString("identify_tty");

IClientCommand::IClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent)
	:QObject(parent)
	,parser(p)
{
	dev=d;
}

IClientCommand* IClientCommand::mkCommand(CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent)
{
	if(p.getArgs().isEmpty())
	{
		qDebug()<<"No command specified";
		return 0;
	}
	QString cmdName=p.getArgs()[0];
	p.getArgsToChange().removeAt(0);
	if(cmdName==listTtyCommand)
		return new ListTtyCommand(p,d,parent);
	else if(cmdName==identifyTtyCommand)
		return new IdentifyTtyCommand(p,d,parent);
	else
	{
		qDebug()<<"Unknown command: "<<cmdName<<"; use "<<qApp->arguments()[0]<<" --help to see help message";
		return 0;
	}
}
