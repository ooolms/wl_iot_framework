#include <QCoreApplication>
#include "IotServer.h"
#include "IotServerDevice.h"
#include "IotServerVirtualDevice.h"
#include "IotServerStorage.h"

//параметры датчика с реального устройства, значения которого мы будем обрабатывать
const QUuid deviceId=QUuid("{f84526c1-5e88-4315-81f8-f7da45daa09d}");
const QByteArray sensorName="blinks_count";

//параметры виртуального устройства, которое будет использоваться для сохранения результатов
//	обработки и обработки команд от пользователя
const QUuid outDeviceId="{129ed0af-42ba-4f4e-a71c-1d0152abf930}";
const QByteArray outSensorsXml="<sensors><sensor type=\"sv_f32\" name=\"result\"/></sensors>";//один датчик - result
const QByteArray outControlsXml=
	"<controls><group title=\"Device controls\">"
	"<control command=\"blink\" title=\"Blink\" button_text=\"blink\"/>"
	"</group></controls>";//одна команда - blink

ARpcISensorStorage *stor=0,*outStor=0;
IotServerVirtualDevice *vDev=0;

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	IotServer srv;
	srv.connection()->startConnectLocal();
	if(!srv.connection()->waitForConnected())
		return 1;

	if(!srv.commands()->devices()->registerVirtualDevice(outDeviceId,"test.cpp-out",outSensorsXml,outControlsXml))
		return 1;
	vDev=srv.devices()->virtualDevById(outDeviceId);
	if(!vDev)return 1;
	QObject::connect(vDev,&IotServerVirtualDevice::processVirtualDeviceCommand,
		[](const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal)
	{
		Q_UNUSED(args)
		if(cmd=="blink")
		{
			qDebug()<<"Wow, we have no led, but we will try";
			qDebug()<<"Blink, BLINK!! Just do this!!!!";
			ok=true;
		}
		else
		{
			retVal.append("unknown command");
			ok=false;
		}
	});

	stor=srv.storages()->existingStorage({deviceId,sensorName});
	if(stor)
	{
		qDebug()<<"db_values_type: "<<stor->storedValuesType().toString();
		qDebug()<<"db_device_name: "<<stor->deviceName();
		qDebug()<<"db_values_count: "<<stor->storedValuesType().toString();
		qDebug()<<"db_store_mode: "<<ARpcISensorStorage::storeModeToString(stor->storeMode());

		QObject::connect(stor,&ARpcISensorStorage::newValueWritten,[](const ARpcSensorValue *v)
		{
			qDebug()<<v->dumpToMsgArgs();
			vDev->sendVDevMeasurement("result",QByteArrayList()<<
				QByteArray::number(v->valueToDouble(0)+qrand()%256-128));
		});

		outStor=srv.storages()->existingStorage({outDeviceId,"result"});
		if(!outStor)
		{
			if(!srv.commands()->storages()->addStorage(outDeviceId.toByteArray(),"result",
				ARpcISensorStorage::LAST_N_VALUES,ARpcISensorStorage::DONT_TOUCH,100))
			{
				qDebug()<<"Can't create output storage";
				return 1;
			}
			outStor=srv.storages()->existingStorage({outDeviceId,"result"});
			if(!outStor)
			{
				qDebug()<<"Can't create output storage";
				return 1;
			}
		}
	}

	QObject::connect(srv.connection(),&IotServerConnection::disconnected,&app,&QCoreApplication::quit);

	return app.exec();
}

