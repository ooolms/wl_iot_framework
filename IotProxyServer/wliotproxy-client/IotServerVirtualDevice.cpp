#include "IotServerVirtualDevice.h"
#include "ARpcBase/ARpcConfig.h"

IotServerVirtualDevice::IotServerVirtualDevice(
	IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,const QByteArray &name,
	const QByteArray &type,QObject *parent)
	:IotServerDevice(conn,cmds,id,name,type,parent)
{
}

void IotServerVirtualDevice::processMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::measurementMsg)
		commands->devices()->sendVDevMeasurement(id(),name(),m.args);
	else if(m.title==ARpcConfig::funcCallMsg)
	{
		if(m.args.count()<2)
		{
			emit newMessage(ARpcMessage(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<"invalid arguments"));
			return;
		}
		QByteArray callId=m.args[0];
		bool ok=false;
		QByteArrayList retVal;
		emit processVirtualDeviceCommand(m.args[1],m.args.mid(2),ok,retVal);
		if(ok)
			emit newMessage(ARpcMessage(ARpcConfig::funcAnswerOkMsg,QByteArrayList()<<callId<<retVal));
		else emit newMessage(ARpcMessage(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<retVal));
	}
	else IotServerDevice::processMessage(m);
}

void IotServerVirtualDevice::sendVDevMeasurement(const QByteArray &sensor,const QByteArrayList &args)
{
	writeMsg(ARpcMessage(ARpcConfig::measurementMsg,QByteArrayList()<<id().toByteArray()<<sensor<<args));
}
