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
#include <QByteArrayList>
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

QString appFilePath;

void showHelp()
{
	qDebug()<<"Usage:\n"<<appFilePath<<" <email> [host]";
	qDebug()<<"\tauthentificate user <email>, if host not set, default is used";
	qDebug()<<appFilePath<<" --set-default-host=<host>";
	qDebug()<<"Set host as default";
	//TODO examples
}

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	appFilePath=QFileInfo(app.arguments()[0]).fileName();
	AlterozoomAuthentificationStorage::readConfig("/var/lib/wliotproxyd/alterozoom_authentification.xml");
	AlterozoomAuthentificationStorage::readConfig("/var/lib/wliotproxyd/proxies.xml");
	if(parser.vars.contains("help"))
	{
		showHelp();
		return 0;
	}
	if(parser.args.isEmpty())
	{
		if(parser.getVarSingle("set-default-host").isEmpty())
		{
			qDebug()<<"Default host: "+AlterozoomAuthentificationStorage::getDefaultHost();
			QMap<QByteArray,QByteArrayList> emailMap;
			for(const auto &s:AlterozoomAuthentificationStorage::getAuthMap().keys())
				emailMap[s.host].append(s.email);
			qDebug()<<"Has tokens:";
			for(auto i=emailMap.begin();i!=emailMap.end();++i)
			{
				qDebug()<<"Host: "<<i.key();
				qDebug()<<"Emails:";
				for(const QByteArray &s:i.value())
					qDebug()<<"\t"<<s;
			}
			return 0;
		}
		else
		{
			AlterozoomAuthentificationStorage::setDefaultHost(parser.getVarSingle("set-default-host").toUtf8());
			qDebug()<<"Done";
			return 0;
		}
	}
	if(!parser.getVarSingle("set-default-host").isEmpty())
		AlterozoomAuthentificationStorage::setDefaultHost(parser.getVarSingle("set-default-host").toUtf8());
	QByteArray email=parser.args[0].toUtf8();
	QByteArray host=AlterozoomAuthentificationStorage::getDefaultHost();
	if(host.isEmpty())
		host="alterozoom.com";
	if(parser.args.count()>1)
	{
		host=AlterozoomAuthentificationStorage::hostFromStr(parser.args[1].toUtf8());
		if(host.isEmpty())return 1;
	}
	qDebug()<<"Host: "<<host;
	if(parser.keys.contains("r"))
	{
		AlterozoomAuthentificationStorage::clearAuth(host,email);
		qDebug()<<"Authentification removed: "<<host<<":"<<email;
		return 0;
	}
	if(!setStdinEchoMode(false))return 1;
	printf("password:");
	std::string s;
	std::cin>>s;
	setStdinEchoMode(true);
	std::cout<<std::endl;
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
	AlterozoomAuthentificationStorage::setAuth(host,email,userId,token);
	return 0;
}
