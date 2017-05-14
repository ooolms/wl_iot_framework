#include "IClientCommand.h"
#include "ListTtyClientCommand.h"
#include "CommandNames.h"
#include <QCoreApplication>
#include <QDebug>

IClientCommand::IClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent)
	:QObject(parent)
	,parser(p)
{
	dev=d;
}

IClientCommand* IClientCommand::mkCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent)
{
	QStringList args=p.getArgs();
	if(args.isEmpty())
	{
		qDebug()<<"No command specified";
		return 0;
	}
	QString cmdName=args[0];
	args.removeAt(0);
	if(cmdName==CommandNames::listTtyCommand)
		return new ListTtyClientCommand(p,d,parent);
	else
	{
		qDebug()<<"Unknown command: "<<cmdName<<"; use "<<qApp->arguments()[0]<<" --help to see help message";
		return 0;
	}
}
