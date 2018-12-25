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
#include <unistd.h>
#include <termios.h>
#include <QDebug>
#include <iostream>
#include <QStringList>
#include "ExternServices/AlterozoomApi.h"
#include "CmdArgParser.h"
#include "ExternServices/AlterozoomAuthentificationStorage.h"
#include <QSettings>
#include <QFileInfo>

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

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	QString fileName=QFileInfo(app.arguments()[0]).fileName();
	if(parser.args.count()<1||parser.vars.contains("help"))
	{
		qDebug()<<"Usage:\n"<<fileName<<" <email> [host]";
		qDebug()<<"\tauthentificate user <email>, if host not set, default is used";
		qDebug()<<fileName<<" --set-default-host=<host>";
		qDebug()<<"Set host as default";
		//TODO examples
		return 0;
	}
	if(!parser.getVarSingle("set-default-host").isEmpty())
	{
		AlterozoomAuthentificationStorage::readConfig("/var/lib/wliotproxyd/alterozoom_authentification.xml");
		AlterozoomAuthentificationStorage::setDefaultHost(parser.getVarSingle("set-default-host").toUtf8());
	}
	QByteArray email=parser.args[0].toUtf8();
	QByteArray host="alterozoom.com";
	if(parser.args.count()>1)
	{
		host=AlterozoomAuthentificationStorage::hostFromStr(parser.args[1].toUtf8());
		qDebug()<<"Host used: "<<host;
	}
	if(!setStdinEchoMode(false))return 1;
	printf("password:");
	std::string s;
	std::cin>>s;
	setStdinEchoMode(true);
	QByteArray pass=QString::fromStdString(s).toUtf8();
	AlterozoomApi api;
	QByteArray token;
	quint64 userId;
	if(!api.authentificate(host,email,pass,token,userId))
	{
		qDebug()<<"Authentification failed";
		return 1;
	}
	qDebug()<<"User identified: "<<userId;
	if(parser.keys.contains("s"))
	{
		AlterozoomAuthentificationStorage::readConfig("/var/lib/wliotproxyd/alterozoom_authentification.xml");
		AlterozoomAuthentificationStorage::setAuth(host,email,userId,token);
	}
	return 0;
}
