#include "IotServerVirtualDevice.h"
#include "wliot/WLIOTConfig.h"

IotServerVirtualDevice::IotServerVirtualDevice(
	IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,const QByteArray &name,
	const QByteArray &type,QObject *parent)
	:IotServerDevice(conn,cmds,id,name,type,parent)
{
}

void IotServerVirtualDevice::processMessage(const Message &m)
{
	if(m.title==WLIOTConfig::funcCallMsg)
	{
		if(m.args.count()<2)
		{
			emit newMessageFromDevice(Message(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<"invalid arguments"));
			return;
		}
		QByteArray callId=m.args[0];
		bool ok=false;
		QByteArrayList retVal;
		emit processVirtualDeviceCommand(m.args[1],m.args.mid(2),ok,retVal);
		if(ok)
			emit newMessageFromDevice(Message(WLIOTConfig::funcAnswerOkMsg,QByteArrayList()<<callId<<retVal));
		else emit newMessageFromDevice(Message(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<retVal));
	}
	else IotServerDevice::processMessage(m);
}

void IotServerVirtualDevice::sendVDevMeasurement(const QByteArray &sensor,const QByteArrayList &args)
{
	commands->devices()->sendVDevMeasurement(id(),sensor,args);
}

void IotServerVirtualDevice::sendVDevMeasurementB(const QByteArray &sensor,const SensorValue &val)
{
	commands->devices()->sendVDevMeasurementB(id(),sensor,val.dumpToBinary());
}
