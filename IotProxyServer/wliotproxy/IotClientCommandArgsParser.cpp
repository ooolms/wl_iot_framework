/*******************************************
   Copyright 2017-2019 OOO "LMS"

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
#include "../wliotproxyd/CmdArgParser.h"
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

bool setStdinEchoMode(bool en)
{
	if(!isatty(STDIN_FILENO))return false;
	struct termios tty;
	if(tcgetattr(STDIN_FILENO,&tty)!=0)return false;
	if(!en)
		tty.c_lflag&=~ECHO;
	else
		tty.c_lflag|=ECHO;
	return tcsetattr(STDIN_FILENO,TCSANOW,&tty)==0;
}

IotClientCommandArgsParser::IotClientCommandArgsParser(int argc,char **argv,QObject *parent)
	:QObject(parent)
	 ,parser(argc,argv)
{
	status=IN_WORK;
	netMode=false;
	QByteArray user,password;
	quint16 netPort=ARpcServerConfig::controlSslPort;
	bool silentMode=parser.keys.contains("compl");
	if(!parser.getVarSingle("net").isEmpty()&&!parser.getVarSingle("user").isEmpty())
	{
		if(silentMode)
			qApp->exit(1);
		user=parser.getVarSingle("user").toUtf8();
		if(!parser.getVarSingle("port").isEmpty())
		{
			netPort=parser.getVarSingle("port").toUShort();
			if(netPort==0)
				netPort=ARpcServerConfig::controlSslPort;
		}
		if(setStdinEchoMode(false))
		{
			std::string s;
			std::cout<<"Enter token:";
			std::cin>>s;
			setStdinEchoMode(true);
			password=QByteArray::fromStdString(s);
		}
		if(!password.isEmpty())
		{
			netMode=true;
			netSock=new QSslSocket(this);
			netSock->setPeerVerifyMode(QSslSocket::VerifyNone);
			dev=new ARpcOutsideDevice(netSock,[this](){netSock->flush();},this);
			connect(netSock,&QSslSocket::encrypted,dev,&ARpcOutsideDevice::onDeviceConnected);
			connect(netSock,&QSslSocket::disconnected,[this]()
			{
				qApp->exit(status==DONE?0:1);
			});
			connect(netSock,static_cast<void (QSslSocket::*)(const QList<QSslError>&)>(&QSslSocket::sslErrors),
				[silentMode](const QList<QSslError> &errs)
			{
				for(const auto &e:errs)
					if(!silentMode)StdQFile::inst().stderrDebug()<<"socket ssl error: "<<e.errorString()<<"\n";
			});
		}
	}
	if(!netMode)
	{
		sock=new QLocalSocket(this);
		dev=new ARpcOutsideDevice(sock,[this](){sock->flush();},this);
		//TODO socket errors
		connect(sock,&QLocalSocket::connected,dev,&ARpcOutsideDevice::onDeviceConnected);
		connect(sock,&QLocalSocket::disconnected,[this]()
		{
			qApp->exit(status==DONE?0:1);
		});
	}
	cmd=0;
	bool showHelp=false;
	QString helpTarget;
	if(parser.args.isEmpty())
		showHelp=true;
	else if(parser.vars.contains("help"))
	{
		showHelp=true;
		helpTarget=parser.getVarSingle("help");
		if(helpTarget.isEmpty()&&parser.args.count()>0)
			helpTarget=parser.args[0];
	}
	else if(parser.args[0]=="help")
	{
		showHelp=true;
		if(parser.args.count()>1)
			helpTarget=parser.args[1];
	}
	if(showHelp)
	{
		ShowHelp::showHelp("",helpTarget);
		status=DONE;
		return;
	}
	if(netMode)
	{
		if(!silentMode)
			StdQFile::inst().stdoutDebug()<<"connecting to remote server: "<<parser.getVarSingle("net")<<"\n";
		netSock->connectToHostEncrypted(parser.getVarSingle("net"),netPort);
		if(!netSock->waitForConnected(5000))
		{
			status=ERROR;
			if(!silentMode)
				StdQFile::inst().stderrDebug()<<"can't connect to IoT server: "<<netSock->errorString()<<"\n";
			return;
		}
		if(!netSock->waitForEncrypted(5000))
		{
			status=ERROR;
			if(!silentMode)
				StdQFile::inst().stderrDebug()<<"can't encrypt connection to IoT server: "<<
					netSock->errorString()<<"\n";
			return;
		}
		QByteArrayList retVal;
		if(execCommand(ARpcServerConfig::authentificateSrvMsg,QByteArrayList()<<user<<password,retVal))
		{
			if(!silentMode)StdQFile::inst().stdoutDebug()<<"authentification done\n";
		}
		else
		{
			if(!silentMode)StdQFile::inst().stderrDebug()<<"authentification failed: "<<retVal.join("|")<<"\n";
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
			if(!silentMode)StdQFile::inst().stderrDebug()<<"can't connect to IoT server\n";
			return;
		}
	}
	QByteArrayList retVal;
	execCommand(ARpcConfig::identifyMsg,QByteArrayList(),retVal);
	if(parser.args[0]=="identify_server")
	{
		if(retVal.count()==2)
		{
			if(silentMode)StdQFile::inst().stdoutDebug()<<retVal[0]<<" "<<retVal[1];
			else StdQFile::inst().stdoutDebug()<<"Server identified: "<<retVal[1]<<" ("<<retVal[0]<<")\n";
			status=DONE;
		}
		else
		{
			StdQFile::inst().stderrDebug()<<"Server is not identified";
			status=ERROR;
		}
		return;
	}
	else
	{
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

bool IotClientCommandArgsParser::execCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	QEventLoop loop;
	QByteArray callId=QByteArray::number(qrand());
	QTimer tmr;
	tmr.setSingleShot(true);
	tmr.setInterval(5000);
	connect(&tmr,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(dev,&ARpcOutsideDevice::disconnected,&loop,&QEventLoop::quit);
	bool done=false;
	auto conn=connect(dev,&ARpcOutsideDevice::newMessage,[this,&loop,&done,&retVal,&callId](const ARpcMessage &m)
	{
		if(m.title==ARpcConfig::funcAnswerOkMsg&&!m.args.isEmpty()&&m.args[0]==callId)
		{
			done=true;
			retVal=m.args.mid(1);
			loop.quit();
		}
		else if(m.title==ARpcConfig::funcAnswerErrMsg&&!m.args.isEmpty()&&m.args[0]==callId)
		{
			retVal=m.args.mid(1);
			loop.quit();
		}
	});
	dev->writeMsg(cmd,QByteArrayList()<<callId<<args);
	tmr.start();
	loop.exec();
	disconnect(conn);
	return done;
}
