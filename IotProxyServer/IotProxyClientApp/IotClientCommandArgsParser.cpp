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
#include "ARpcBase/ARpcServerConfig.h"
#include <QThread>
#include <QCoreApplication>
#include <QLocalSocket>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <QDebug>

const QString localServerName=QString("wliotproxyd");

void setStdinEchoMode(bool en)
{
	struct termios tty;
	tcgetattr(STDIN_FILENO,&tty);
	if(!en)
		tty.c_lflag&=~ECHO;
	else
		tty.c_lflag|=ECHO;
	(void)tcsetattr(STDIN_FILENO,TCSANOW,&tty);
}

IotClientCommandArgsParser::IotClientCommandArgsParser(int argc,char **argv,QObject *parent)
	:QObject(parent)
	 ,parser(argc,argv)
{
	status=IN_WORK;
	netMode=false;
	QString token;
	if(parser.hasVar("net"))
	{
		if(parser.hasVar("token"))
			token=parser.getVarSingle("token");
		else
		{
			setStdinEchoMode(false);
			std::cout<<"Enter token: ";
			std::string s;
			std::cin>>s;
			setStdinEchoMode(true);
			token=QString::fromStdString(s);
		}
		if(!token.isEmpty())
		{
			netMode=true;
			netSock=new QSslSocket(this);
			netSock->setPeerVerifyMode(QSslSocket::VerifyNone);
			dev=new ARpcOutsideDevice(netSock,this);
			connect(netSock,&QSslSocket::encrypted,dev,&ARpcOutsideDevice::onDeviceConnected);
			connect(netSock,&QSslSocket::disconnected,[this]()
			{
				qApp->exit(status==DONE?0:1);
			});
			connect(netSock,static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
				[](const QList<QSslError> &errs)
			{
				for(const auto &e:errs)
					StdQFile::inst().stderrDebug()<<"socket ssl error: "<<e.errorString()<<"\n";
			});
		}
		//CRIT socket errors
		//		connect(netSock,&QSslSocket::error,dev,&ARpcOutsideDevice::);
	}
	if(!netMode)
	{
		sock=new QLocalSocket(this);
		dev=new ARpcOutsideDevice(sock,this);
		//CRIT socket errors
		connect(sock,&QLocalSocket::connected,dev,&ARpcOutsideDevice::onDeviceConnected);
		connect(sock,&QLocalSocket::disconnected,[this]()
		{
			qApp->exit(status==DONE?0:1);
		});
	}
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
	if(netMode)
	{
		StdQFile::inst().stdoutDebug()<<"connecting to remote server: "<<parser.getVarSingle("net")<<"\n";
		netSock->connectToHostEncrypted(parser.getVarSingle("net"),ARpcServerConfig::controlSslPort);
		if(!netSock->waitForConnected(5000))
		{
			status=ERROR;
			StdQFile::inst().stderrDebug()<<"can't connect to IoT server: "<<netSock->errorString()<<"\n";
			return;
		}
		if(!netSock->waitForEncrypted(5000))
		{
			status=ERROR;
			StdQFile::inst().stderrDebug()<<"can't encrypt connection to IoT server: "<<netSock->errorString()<<"\n";
			return;
		}
		QStringList retVal;
		if(execCommand(ARpcMessage(ARpcServerConfig::authentificateSrvMsg,QStringList()<<token),retVal))
			StdQFile::inst().stdoutDebug()<<"authentification done\n";
		else
		{
			StdQFile::inst().stderrDebug()<<"authentification failed: "<<retVal.join("|")<<"\n";
			status=ERROR;
			return;
		}
	}
	else
	{
		sock->connectToServer(localServerName);
		if(!sock->waitForConnected())
		{
			status=ERROR;
			StdQFile::inst().stderrDebug()<<"can't connect to IoT server\n";
			return;
		}
	}
	QStringList retVal;
	if(execCommand(ARpcMessage(ARpcConfig::identifyMsg),retVal)&&retVal.count()==2)
		StdQFile::inst().stdoutDebug()<<"Server identified: "<<retVal[1]<<" ("<<retVal[0]<<")\n";
	cmd=IClientCommand::mkCommand(parser,dev);
	if(!cmd)
	{
		status=ERROR;
		return;
	}
	QThread::usleep(100);
	if(!cmd->evalCommand())
		status=ERROR;
}

IotClientCommandArgsParser::~IotClientCommandArgsParser()
{
	if(cmd)
		delete cmd;
}

IotClientCommandArgsParser::CommandStatus IotClientCommandArgsParser::getCommandStatus()
{
	return status;
}

bool IotClientCommandArgsParser::execCommand(const ARpcMessage &m,QStringList &retVal)
{
	QEventLoop loop;
	QTimer tmr;
	tmr.setSingleShot(true);
	tmr.setInterval(5000);
	connect(&tmr,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(dev,&ARpcOutsideDevice::disconnected,&loop,&QEventLoop::quit);
	bool done=false;
	auto conn=connect(dev,&ARpcOutsideDevice::rawMessage,[this,&loop,&done,&retVal](const ARpcMessage &m)
	{
		if(m.title==ARpcConfig::funcAnswerOkMsg)
		{
			done=true;
			retVal=m.args;
			loop.quit();
		}
		else if(m.title==ARpcConfig::funcAnswerErrMsg)
		{
			retVal=m.args;
			loop.quit();
		}
	});
	dev->writeMsg(m);
	tmr.start();
	loop.exec();
	disconnect(conn);
	return done;
}
