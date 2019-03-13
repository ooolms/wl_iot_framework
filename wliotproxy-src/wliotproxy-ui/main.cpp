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

#include <QApplication>
#include "ControlUiWidget.h"
#include "CmdArgParser.h"
#include "IotServer.h"
#include <QDebug>

//TODO parse cmd args params: --host,--port,--user,--password
int main(int argc,char *argv[])
{
	QApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	if(parser.args.count()<1)return 1;
	if(parser.vars.contains("--help"))
	{
		qDebug()<<"Usage: "<<qApp->arguments()[0]<<" <device_id_or_name>";
		return 0;
	}
	QString devIdOrName=parser.args[0];

	QString host=parser.getVarSingle("host");
	QByteArray user=parser.getVarSingle("user").toUtf8();
	QByteArray pass=parser.getVarSingle("pass").toUtf8();
	quint16 port=WLIOTServerProtocolDefs::controlSslPort;
	if(!parser.getVarSingle("port").isEmpty())
		port=parser.getVarSingle("port").toUShort();
	if(port==0)
		port=WLIOTServerProtocolDefs::controlSslPort;
	bool netMode=!host.isEmpty()&&!user.isEmpty();
	//создаем объект IotServer и подключаемся к серверу

	IotServer srv;
	if(!netMode)
		srv.connection()->startConnectLocal();
	else srv.connection()->startConnectNet(host,port);
	if(!srv.connection()->waitForConnected())
		return __LINE__;
	if(netMode&&!srv.connection()->authentificateNet(user,pass))
		return __LINE__;
	if(!netMode&&!user.isEmpty()&&!srv.connection()->authentificateLocalFromRoot(user))
		return __LINE__;

	RealDevice *dev=srv.devices()->findDevByIdOrName(devIdOrName.toUtf8());
	if(!dev)
	{
		qCritical()<<"No device found";
		return 1;
	}
	ControlUiWidget w(dev);
	if(!w.prepare())return 1;
	w.show();
	return app.exec();
}

