#include <QCoreApplication>
#include <QTcpSocket>
#include <QUdpSocket>
#include <string.h>
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include "ARpcSrvReady.h"
#include "ARpcDevice.h"

QTcpSocket client;
QUdpSocket bCastClient;
unsigned int blinksCount=0;//число миганий
bool wasSync=false;
QHostAddress bCastSenderIp;
const quint16 port=4081;
QTimer syncTimer;

const char *deviceName="led_blink_test";//имя устройства
const ARpcUuid deviceId("f84526c15e88431581f8f7da45daa09d");//идентификатор устройства

//Описание интерфейса управления
const char *interfaceStr="<controls><group title=\"Device controls\">"
"<control command=\"blink\" sync=\"0\" title=\"blink\" force_button=\"1\">"//команда blink
"<param type=\"slider\" title=\"delay\"><attributes max=\"1000\" min=\"100\"/></param>"//параметр - время горения в мс
"</control>"
"<control command=\"get_blinks_count\" title=\"get_blinks_count\"/>"//команда get_blinks_count
"</group></controls>";

//Описание датчиков
const char *sensorsDef="<sensors>"
"<sensor name=\"blinks_count\" type=\"f32_sv\"/>"//датчик blinks_count
"</sensors>";

class NetWriteCb
	:public ARpcIWriteCallback
{
public:
	void writeData(const char *d,unsigned long sz)
	{
		client.write(d,sz);
	}

	void writeStr(const char *str)
	{
		client.write(QByteArray(str));
	}
}netCb;

ARpcDevice wifiDev(300,&netCb,&deviceId,deviceName);

//callback-класс для обработки команд, вызывается библиотекой ARpc
class EventsCb
	:public ARpcIDevEventsCallback
{
public:
	explicit EventsCb(ARpcDevice *d)
	{
		dev=d;
	}

	void blink(int dl)
	{
		qDebug()<<"Blink command";
	}

	virtual void processCommand(const char *cmd,const char *args[],unsigned char argsCount)
	{
		if(strcmp(cmd,"blink")==0&&argsCount>=1)//команда blink, проверяем что есть аргумент
		{
			int dl=QByteArray(args[0]).toInt();//аргумент - время горения светодиода в мс
			//правим - от 100 до 1000 мс
			if(dl<100)dl=100;
			else if(dl>1000)dl=1000;
			//мигаем
			blink(dl);
			//инкрементируем число миганий
			++blinksCount;
			//выдаем измерение
			dev->disp().writeMeasurement("blinks_count",QByteArray::number(blinksCount).constData());
			//сообщаем об успешном выполнении команды
			dev->disp().writeOk();
		}
		else if(strcmp(cmd,"get_blinks_count")==0)//команда get_blinks_count
		{
			//выдаем измерение
			dev->disp().writeMeasurement("blinks_count",QByteArray::number(blinksCount).constData());
			//сообщаем об успешном выполнении команды
			dev->disp().writeOk();
		}
		else dev->disp().writeErr("Unknown cmd");//неизвестная команда
	}

	virtual void onSyncMsg()
	{
		wasSync=true;
		qDebug()<<"sync";
	}

private:
    ARpcDevice *dev;
}wifiEcb(&wifiDev);

class SrcReadyCb
	:public ARpcISrvReadyCallback
{
public:
	void processSrvReadyMsg(const ARpcUuid &srvId,const char *srvName)
	{
		if(client.state()==QTcpSocket::ConnectedState||
			client.state()==QTcpSocket::ConnectingState)return;
		char str[39];
		srvId.toString(str);
		qDebug()<<"Server detected: "<<str<<":"<<srvName;
		client.connectToHost(bCastSenderIp,port);
		if(client.waitForConnected())
			qDebug()<<"Connected to server";
		else qDebug()<<"Connection error: "<<client.errorString();
		wifiDev.resetParser();
		wasSync=true;
	}
}srvReadyCb;

ARpcSrvReady srvReadyParser(200,&srvReadyCb);

void setup()
{
	wifiDev.disp().installDevEventsHandler(&wifiEcb);
	wifiDev.disp().setControls(interfaceStr);
	wifiDev.disp().setSensors(sensorsDef);

	bCastClient.bind(port);

	syncTimer.setInterval(6000);

	QObject::connect(&client,&QTcpSocket::readyRead,[&client,&wifiDev]()
	{
		QByteArray data=client.readAll();
		wifiDev.putData(data.constData(),data.size());
	});
	QObject::connect(&client,&QTcpSocket::disconnected,[]()
	{
		qDebug()<<"Client connection lost";
	});
	QObject::connect(&bCastClient,&QUdpSocket::readyRead,[&bCastClient,&srvReadyParser]()
	{
		while(bCastClient.hasPendingDatagrams())
		{
			QByteArray data;
			data.resize(bCastClient.pendingDatagramSize());
			bCastClient.readDatagram(data.data(),data.size(),&bCastSenderIp);
			srvReadyParser.putData(data.constData(),data.size());
		}
	});
	QObject::connect(&syncTimer,&QTimer::timeout,[&client,&wasSync]()
	{
		if(wasSync)
			wasSync=false;
		else
		{
			client.disconnectFromHost();
			qDebug()<<"Sync timeout";
		}
	});
}

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	setup();
	return app.exec();
}
