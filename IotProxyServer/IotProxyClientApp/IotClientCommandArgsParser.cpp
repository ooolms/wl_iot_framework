#include "IotClientCommandArgsParser.h"
#include "CmdArgParser.h"
#include "ShowHelp.h"
#include "StdQFile.h"
#include <QCoreApplication>
#include <QLocalSocket>
#include <QDebug>

const QString localServerName=QString("WLIotProxyServer");

IotClientCommandArgsParser::IotClientCommandArgsParser(int argc,char **argv,QObject *parent)
	:QObject(parent)
	,parser(argc,argv)
{
	status=IN_WORK;
	sock=new QLocalSocket(this);
	dev=new ARpcOutsideDevice(sock,this);
	cmd=0;

	//CRIT process --help key
	//TODO err codes?
	if(parser.hasVar("help"))
	{
		if(parser.getArgs().isEmpty())
		{
			ShowHelp::showHelp("","");
			status=DONE;
			return;
		}
		else
		{
			ShowHelp::showHelp("",parser.getArgs()[0]);
			status=DONE;
			return;
		}
	}
	if(parser.getArgs().isEmpty())
	{
		ShowHelp::showHelp("","");
		status=DONE;
		return;
	}
	sock->connectToServer(localServerName);
	if(!sock->waitForConnected())
	{
		status=ERROR;
		QDebug(StdQFile::inst().stderr())<<"Can't connect to IoT server\n";
		return;
	}
	cmd=IClientCommand::mkCommand(parser,dev);
	if(!cmd)
	{
		status=ERROR;
		return;
	}
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IotClientCommandArgsParser::onRawMessage);
	connect(sock,&QLocalSocket::disconnected,[this]()
	{
		qApp->exit(1);
	});
	if(!cmd->evalCommand())
		status=ERROR;
}

IotClientCommandArgsParser::~IotClientCommandArgsParser()
{
	if(cmd)delete cmd;
}

IotClientCommandArgsParser::CommandStatus IotClientCommandArgsParser::getCommandStatus()
{
	return status;
}

void IotClientCommandArgsParser::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::infoMsg)
	{
		QDebug(StdQFile::inst().stdout())<<m.args.join("|")<<"\n";
	}
	else if(m.title==ARpcConfig::funcAnswerOkMsg)
	{
		QDebug(StdQFile::inst().stdout())<<"Ok\n";
		qApp->exit(0);
	}
	else if(m.title==ARpcConfig::funcAnswerErrMsg)
	{
		QDebug(StdQFile::inst().stdout())<<"ERROR: "<<m.args.join("|")<<"\n";
		qApp->exit(0);
	}
}
