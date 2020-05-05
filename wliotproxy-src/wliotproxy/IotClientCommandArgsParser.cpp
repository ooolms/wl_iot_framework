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
#include "wliot/WLIOTServerProtocolDefs.h"
#include <QThread>
#include <QCoreApplication>
#include <QLocalSocket>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <QDebug>

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
	bool netMode=false;
	QByteArray user,password;
	QString host;
	quint16 netPort=WLIOTServerProtocolDefs::controlSslPort;
	bool silentMode=parser.keys.contains("compl");
	conn=new IotServerConnection(this);
	conn->setNoDebug(silentMode);
	if(!parser.getVarSingle("host").isEmpty()&&!parser.getVarSingle("user").isEmpty())
	{
		netMode=true;
		host=parser.getVarSingle("host");
		if(silentMode)
			qApp->exit(1);
		user=parser.getVarSingle("user").toUtf8();
		if(!parser.getVarSingle("port").isEmpty())
		{
			netPort=parser.getVarSingle("port").toUShort();
			if(netPort==0)
				netPort=WLIOTServerProtocolDefs::controlSslPort;
		}
		if(setStdinEchoMode(false))
		{
			std::string s;
			std::cout<<"Authentication password:";
			std::cin>>s;
			setStdinEchoMode(true);
			password=QByteArray::fromStdString(s);
		}
		if(password.isEmpty())
		{
			status=AUTHENTICATE_ERROR;
			return;
		}
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
			StdQFile::inst().stdoutDebug()<<"connecting to remote server: "<<host<<"\n";
		conn->startConnectNet(host,netPort);
		if(!conn->waitForConnected())
		{
			if(!silentMode)StdQFile::inst().stdoutDebug()<<"remote connection failed\n";
			status=CONNECTION_ERROR;
			return;
		}
		QByteArrayList retVal;
		if(conn->authenticateNet(user,password))
		{
			if(!silentMode)StdQFile::inst().stdoutDebug()<<"authentication done\n";
		}
		else
		{
			if(!silentMode)StdQFile::inst().stderrDebug()<<"authentication failed: "<<retVal.join("|")<<"\n";
			status=AUTHENTICATE_ERROR;
			return;
		}
	}
	else
	{
		conn->startConnectLocal();
		if(!conn->waitForConnected())
		{
			if(!silentMode)StdQFile::inst().stdoutDebug()<<"local connection failed\n";
			status=CONNECTION_ERROR;
			return;
		}
		if(!parser.getVarSingle("user").isEmpty())
		{
			if(!conn->authenticateLocalFromRoot(parser.getVarSingle("user").toUtf8()))
			{
				if(!silentMode)StdQFile::inst().stdoutDebug()<<"no user found"<<parser.getVarSingle("user")<<"\n";
				status=AUTHENTICATE_ERROR;
				return;
			}
		}
	}
	if(parser.args[0]=="identify_server")
	{
		QUuid srvId;
		QByteArray srvName;
		if(conn->identifyServer(srvId,srvName))
		{
			if(silentMode)StdQFile::inst().stdoutDebug()<<srvId.toString()<<" "<<QString::fromUtf8(srvName);
			else StdQFile::inst().stdoutDebug()<<"Server identified: "<<srvId.toString()<<
				" ("<<QString::fromUtf8(srvName)<<")\n";
			status=DONE;
		}
		else
		{
			StdQFile::inst().stderrDebug()<<"Server is not identified";
			status=COMMAND_ERROR;
		}
		return;
	}
	else
	{
		cmd=IClientCommand::mkCommand(parser,conn);
		if(!cmd)
		{
			status=ARGS_PARSING_ERROR;
			return;
		}
//		QThread::msleep(100);
		if(!cmd->evalCommand())
			status=COMMAND_ERROR;
	}
	connect(conn,&IotServerConnection::disconnected,qApp,&QCoreApplication::quit);
}

IotClientCommandArgsParser::~IotClientCommandArgsParser()
{
	if(cmd)
		delete cmd;
	delete conn;
}

IotClientCommandArgsParser::CommandStatus IotClientCommandArgsParser::getCommandStatus()
{
	return status;
}
