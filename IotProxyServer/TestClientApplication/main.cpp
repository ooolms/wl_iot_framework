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
#include <termios.h>
#include <unistd.h>
#include "IotServer.h"
#include "IotServerDevice.h"
#include "IotServerVirtualDevice.h"
#include "IotServerStorage.h"
#include "CmdArgParser.h"

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

QUuid deviceId=QUuid("{3a4b236c-1da6-4332-9c43-86c83bfbae4e}");
QByteArray deviceName="TestDev";
IotServerVirtualDevice *vDev=0;

//функция, генерирующая список датчиков
QList<ARpcSensorDef> mkSensors()
{
	QList<ARpcSensorDef> retVal;
	retVal.append(ARpcSensorDef(ARpcSensorDef::Type(ARpcSensorDef::F32,ARpcSensorDef::SINGLE,ARpcSensorDef::NO_TIME,1),
		"temp"));
	retVal.append(ARpcSensorDef(ARpcSensorDef::Type(ARpcSensorDef::F32,ARpcSensorDef::SINGLE,ARpcSensorDef::NO_TIME,1),
		"hum"));
	return retVal;
}

ARpcControlsGroup mkControls()
{
	ARpcControlsGroup grp;
	ARpcCommandControl cmd;
	cmd.buttonText="set_led";
	cmd.command="blink";
	cmd.title="Blink";
	grp.elements.append(ARpcControlsGroup::Element(cmd));
	return grp;
}

void onTimer()
{
	vDev->sendVDevMeasurement("temp",QByteArrayList()<<QByteArray::number(20.0+(qrand()%10)/10.0));
	vDev->sendVDevMeasurement("hum",QByteArrayList()<<QByteArray::number(35.0+(qrand()%10)));
}

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
	QString host=parser.getVarSingle("host");
	QByteArray user=parser.getVarSingle("user").toUtf8();
	QByteArray pass=parser.getVarSingle("pass").toUtf8();
	quint16 port=ARpcServerConfig::controlSslPort;
	if(!parser.getVarSingle("port").isEmpty())
		port=parser.getVarSingle("port").toUShort();
	if(port==0)
		port=ARpcServerConfig::controlSslPort;
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

	if(!parser.getVarSingle("uuid").isEmpty())
	{
		deviceId=QUuid(parser.getVarSingle("uuid"));
		if(deviceId.isNull())return 1;
	}
	if(!parser.getVarSingle("name").isEmpty())
		deviceName=parser.getVarSingle("name").toUtf8();

	//регистрируем виртуальное устройство
	if(!srv.devices()->registerVirtualDevice(deviceId,deviceName,mkSensors(),mkControls()))
		return 1;
	vDev=srv.devices()->virtualDevById(deviceId);
	if(!vDev)return 1;

	//устанавливаем обработчик команд
	QObject::connect(vDev,&IotServerVirtualDevice::processVirtualDeviceCommand,
		[](const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal)
	{
		Q_UNUSED(args)
		if(cmd=="blink")
		{
			//команда blink
			qDebug()<<"Wow, we have no led, but we will try!";
			ok=true;
		}
		else
		{
			//неизвестная команда
			retVal.append("unknown command");
			ok=false;
		}
	});

	//завершаем работу в случае обрыва соединения с сервером
	QObject::connect(srv.connection(),&IotServerConnection::disconnected,&app,&QCoreApplication::quit);

	QTimer timer;
	timer.setInterval(60000);
	timer.setSingleShot(false);
	QObject::connect(&timer,&QTimer::timeout,&onTimer);
	timer.start();

	int r=app.exec();
	timer.stop();
	return r;
}

