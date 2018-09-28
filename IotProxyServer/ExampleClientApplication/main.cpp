#include <QCoreApplication>
#include "IotServer.h"
#include "IotServerDevice.h"
#include "IotServerVirtualDevice.h"
#include "IotServerStorage.h"

//параметры датчика с реального устройства, значения которого мы будем обрабатывать
const QUuid deviceId=QUuid("{f84526c1-5e88-4315-81f8-f7da45daa09d}");
const QByteArray sensorName="blinks_count";

//идентификатор виртуального устройства, которое будет использоваться для сохранения результатов
//	обработки и обработки команд от пользователя
const QUuid outDeviceId="{129ed0af-42ba-4f4e-a71c-1d0152abf930}";

//хранилище датчика с реального устройства
ARpcISensorStorage *inStor=0;

//виртуальное устройство
IotServerVirtualDevice *vDev=0;

//функция, генерирующая список датчиков
QList<ARpcSensorDef> mkSensors()
{
	QList<ARpcSensorDef> retVal;
	retVal.append(ARpcSensorDef(ARpcSensorDef::Type(ARpcSensorDef::F32,ARpcSensorDef::SINGLE,ARpcSensorDef::NO_TIME,1),
		"result"));
	return retVal;
}

ARpcControlsGroup mkControls()
{
	ARpcControlsGroup grp;
	ARpcCommandControl cmd;
	cmd.buttonText="blink";
	cmd.command="blink";
	cmd.title="Blink";
	grp.elements.append(ARpcControlsGroup::Element(cmd));
	return grp;
}

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	//создаем объект IotServer и подключаемся к серверу
	IotServer srv;
	srv.connection()->startConnectLocal();
	if(!srv.connection()->waitForConnected())
		return 1;

	//регистрируем виртуальное устройство
	if(!srv.devices()->registerVirtualDevice(outDeviceId,"test.cpp-out",mkSensors(),mkControls()))
		return 1;
	vDev=srv.devices()->virtualDevById(outDeviceId);
	if(!vDev)return 1;

	//устанавливаем обработчик команд
	QObject::connect(vDev,&IotServerVirtualDevice::processVirtualDeviceCommand,
		[](const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal)
	{
		Q_UNUSED(args)
		if(cmd=="blink")
		{
			//команда blink
			qDebug()<<"Wow, we have no led, but we will try";
			qDebug()<<"Blink, BLINK!! Just do this!!!!";
			ok=true;
		}
		else
		{
			//неизвестная команда
			retVal.append("unknown command");
			ok=false;
		}
	});

	//ищем хранилище для реального датчика
	inStor=srv.storages()->existingStorage({deviceId,sensorName});
	if(inStor)
	{
		qDebug()<<"db_values_type: "<<inStor->storedValuesType().toString();
		qDebug()<<"db_device_name: "<<inStor->deviceName();
		qDebug()<<"db_values_count: "<<inStor->storedValuesType().toString();
		qDebug()<<"db_store_mode: "<<ARpcISensorStorage::storeModeToString(inStor->storeMode());

		//устанавливаем обработчик на появление нового значения
		QObject::connect(inStor,&ARpcISensorStorage::newValueWritten,[](const ARpcSensorValue *v)
		{
			qDebug()<<v->dumpToMsgArgs();
			//"обрабатываем" значение и выдаем результат как измерение с виртуального устройства
			double processedValue=v->valueToDouble(0)+qrand()%256-128;
			vDev->sendVDevMeasurement("result",QByteArrayList()<<QByteArray::number(processedValue));
		});

		//ищем хранилище для датчика с виртуального устройства
		ARpcISensorStorage *outStor=srv.storages()->existingStorage({outDeviceId,"result"});
		if(!outStor)
		{
			//если отсутствует, создаем
			if(!srv.commands()->storages()->addStorage(outDeviceId.toByteArray(),"result",
				ARpcISensorStorage::LAST_N_VALUES,ARpcISensorStorage::DONT_TOUCH,100))
			{
				qDebug()<<"Can't create output storage";
				return 1;
			}
		}
	}

	//завершаем работу в случае обрыва соединения с сервером
	QObject::connect(srv.connection(),&IotServerConnection::disconnected,&app,&QCoreApplication::quit);

	return app.exec();
}

