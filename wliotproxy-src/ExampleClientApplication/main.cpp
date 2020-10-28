#include <QCoreApplication>
#include "wliot/client/ServerInstance.h"
#include "wliot/client/VirtualDeviceClient.h"
#include "wliot/client/VirtualDeviceCallback.h"
#include "CmdArgParser.h"

using namespace WLIOTClient;
using namespace WLIOT;

//параметры датчика с реального устройства, значения которого мы будем обрабатывать
const QUuid deviceId=QUuid("{f84526c1-5e88-4315-81f8-f7da45daa09d}");
const QByteArray sensorName="blinks_count";

//идентификатор виртуального устройства, которое будет использоваться для сохранения результатов
//	обработки и обработки команд от пользователя
const QUuid outDeviceId="{129ed0af-42ba-4f4e-a71c-1d0152abf930}";

//хранилище датчика с реального устройства
ISensorStorage *inStor=0;

//виртуальное устройство
VirtualDeviceClient *vDev=0;

//функция, генерирующая список датчиков
QList<SensorDef> mkSensors()
{
	QList<SensorDef> retVal;
	retVal.append(SensorDef(SensorDef::Type(SensorDef::F32,SensorDef::SINGLE,SensorDef::NO_TIME,1),
		"result"));
	return retVal;
}

ControlsGroup mkControls()
{
	ControlsGroup grp;
	ControlsCommand cmd;
	cmd.buttonText="blink";
	cmd.commandToExec="blink";
	cmd.title="Blink";
	grp.elements.append(ControlsGroup::Element(cmd));
	return grp;
}

class VDevCallback
	:public VirtualDeviceCallback
{
public:
	virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)override
	{
		Q_UNUSED(args)
		Q_UNUSED(retVal)
		if(cmd=="blink")
		{
			//команда blink
			qDebug()<<"Blink command";
			return true;
		}
		else
		{
			//неизвестная команда
			retVal.append("unknown command");
			return false;
		}
	}
};

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	CmdArgParser parser(app.arguments());
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
	ServerInstance srv;
	if(!user.isEmpty())
		srv.connection()->prepareAuth(user,pass);
	if(!netMode)
		srv.connection()->startConnectLocal();
	else srv.connection()->startConnectNet(host,port);
	if(!srv.connection()->waitForConnected())
		return __LINE__;
	if(!srv.connection()->isReady())
		return __LINE__;

	//регистрируем виртуальное устройство
	VDevCallback cb;
	vDev=srv.devices()->registerVirtualDevice(outDeviceId,"test.cpp-out",mkSensors(),mkControls(),QUuid(),&cb);
	if(!vDev)
		return __LINE__;

	//ищем хранилище для реального датчика
	inStor=srv.storages()->existingStorage({deviceId,sensorName});
	if(inStor)
	{
		qDebug()<<"db_values_type: "<<inStor->storedValuesType().toString();
		qDebug()<<"db_device_name: "<<inStor->deviceName();
		qDebug()<<"db_values_count: "<<inStor->storedValuesType().toString();
		qDebug()<<"db_store_mode: "<<ISensorStorage::storeModeToString(inStor->storeMode());

		//устанавливаем обработчик на появление нового значения
		QObject::connect(inStor,&ISensorStorage::newValueWritten,[](const SensorValue *v)
		{
			qDebug()<<v->dumpToMsgArgs();
			//"обрабатываем" значение и выдаем результат как измерение с виртуального устройства
			double processedValue=v->valueToDouble(0)+qrand()%256-128;
			vDev->sendVDevMeasurement("result",QByteArrayList()<<QByteArray::number(processedValue));
		});

		//ищем хранилище для датчика с виртуального устройства
		ISensorStorage *outStor=srv.storages()->existingStorage({outDeviceId,"result"});
		if(!outStor)
		{
			//если отсутствует, создаем
			if(!srv.commands()->storages()->addStorage(outDeviceId.toByteArray(),"result",
				ISensorStorage::LAST_N_VALUES,ISensorStorage::DONT_TOUCH,100))
			{
				qDebug()<<"Can't create output storage";
				return __LINE__;
			}
		}
	}

	//завершаем работу в случае обрыва соединения с сервером
	QObject::connect(srv.connection(),&ServerConnection::disconnected,&app,&QCoreApplication::quit);

	return app.exec();
}

