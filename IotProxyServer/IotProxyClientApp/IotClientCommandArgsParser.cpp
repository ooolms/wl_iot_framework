#include "IotClientCommandArgsParser.h"
#include "CmdArgParser.h"
#include <QCoreApplication>
#include <QLocalSocket>
#include <QDebug>

const QString localServerName=QString("WLIotProxyServer");

IotClientCommandArgsParser::IotClientCommandArgsParser(int argc,char **argv,QObject *parent)
	:QObject(parent)
	,parser(argc,argv)
{
	status=true;
	sock=new QLocalSocket(this);
	dev=new ARpcOutsideDevice(sock,this);

	//CRIT process --help key
	//TODO err codes?
	if(parser.getArgs().isEmpty())
	{
		status=false;
		return;
	}
	sock->connectToServer(localServerName);
	if(!sock->waitForConnected())
	{
		status=false;
		qDebug()<<"Can't connect to IoT server";
		return;
	}
	cmd=IClientCommand::mkCommand(parser,dev);
	if(!cmd)
	{
		status=false;
		return;
	}
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IotClientCommandArgsParser::onRawMessage);
	connect(sock,&QLocalSocket::disconnected,[this]()
	{
		qApp->exit(1);
	});
	if(!cmd->evalCommand())
		status=false;
}

IotClientCommandArgsParser::~IotClientCommandArgsParser()
{
	delete cmd;
}

bool IotClientCommandArgsParser::getCommandStatus()
{
	return status;
}

void IotClientCommandArgsParser::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::infoMsg)
	{
		qDebug()<<m.args.join("|");
	}
	else if(m.title==ARpcConfig::funcAnswerOkMsg)
	{
		qDebug()<<"Ok";
		qApp->exit(0);
	}
	else if(m.title==ARpcConfig::funcAnswerErrMsg)
	{
		qDebug()<<"ERROR: "<<m.args.join("|");
		qApp->exit(0);
	}
}
