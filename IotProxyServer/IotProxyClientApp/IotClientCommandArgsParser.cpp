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

#include "IotClientCommandArgsParser.h"
#include "CmdArgParser.h"
#include "ShowHelp.h"
#include "StdQFile.h"
#include <QThread>
#include <QCoreApplication>
#include <QLocalSocket>
#include <QDebug>

const QString localServerName=QString("wliotproxyd");

IotClientCommandArgsParser::IotClientCommandArgsParser(int argc,char **argv,QObject *parent)
	:QObject(parent)
	,parser(argc,argv)
{
	status=IN_WORK;
	sock=new QLocalSocket(this);
	dev=new ARpcOutsideDevice(sock,this);
	connect(sock,&QLocalSocket::connected,dev,&ARpcOutsideDevice::onDeviceOpened);
	cmd=0;

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
		StdQFile::inst().stderrDebug()<<"Can't connect to IoT server\n";
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
	QThread::usleep(100);
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

}
