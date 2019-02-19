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

#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include "CmdArgParser.h"
#include "IotProxyConfig.h"

const char *helpString=
"Usage: wliotproxy-admin <command> [args]\n"
"Commands:\n"
"add_user <user_name>\n"
"del_user <user_name>\n"
"passwd <user_name>\n"
;

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

QByteArray readPassword()
{
	setStdinEchoMode(false);
	std::cout<<"password: ";
	std::string s1,s2;
	std::cin>>s1;
	std::cout<<"\nrepeat:";
	std::cin>>s2;
	std::cout<<"\n";
	setStdinEchoMode(true);
	if(s1!=s2)
	{
		std::cout<<"don't match";
		return QByteArray();
	}
	return QByteArray::fromStdString(s1);
}

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(argc,argv);
	if(parser.args.count()<2||parser.vars.contains("help"))
	{
		qDebug()<<helpString;
		return 0;
	}
	if(!IotProxyConfig::readConfig(parser))
	{
		qDebug()<<"Can't read config";
		return 1;
	}
	QString cmd=parser.args[0];
	QByteArray userName=parser.args[1].toUtf8();
	if(cmd=="add_user")
	{
		qint32 uid;
		if(!IotProxyConfig::accessManager.createUser(userName,uid))
		{
			qDebug()<<"User already exists or user name is invalid (only a-z,A-Z,0-9,'.','-' and '_' are allowed)";
			return 1;
		}
		QByteArray pass=readPassword();
		if(pass.isEmpty())return 0;
		if(!IotProxyConfig::accessManager.userSetPass(uid,pass))
		{
			qDebug()<<"User not found";
			return 1;
		}
	}
	else if(cmd=="del_user")
		IotProxyConfig::accessManager.delUser(userName);
	else if(cmd=="passwd")
	{
		QByteArray pass=readPassword();
		if(pass.isEmpty())return 1;
		IdType uid=IotProxyConfig::accessManager.userId(userName);
		if(uid==nullId)return 1;
		if(!IotProxyConfig::accessManager.userSetPass(uid,pass))
		{
			qDebug()<<"User not found";
			return 1;
		}
	}
	else
	{
		qDebug()<<"Invalid operation";
		qDebug()<<helpString;
		return 1;
	}
	return 0;
}
