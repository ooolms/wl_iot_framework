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
	else IotServerDevice::processMessage(m);
}

void IotServerVirtualDevice::sendVDevMeasurement(const QByteArray &sensor,const QByteArrayList &args)
{
	writeMsg(ARpcMessage(ARpcConfig::measurementMsg,QByteArrayList()<<sensor<<args));
}
