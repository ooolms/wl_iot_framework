#include "ARpcOutsideDeviceWrap.h"

ARpcOutsideDeviceWrap::ARpcOutsideDeviceWrap(
	const QByteArray &devIdOrName,ARpcOutsideDevice *srvSocketDev,QObject *parent)
	:ARpcRealDevice(parent)
{
	deviceIdOrName=devIdOrName;
	srvSock=srvSocketDev;
	connect(srvSocketDev,&ARpcOutsideDevice::connected,this,&ARpcOutsideDeviceWrap::connected);
	connect(srvSocketDev,&ARpcOutsideDevice::disconnected,this,&ARpcOutsideDeviceWrap::disconnected);
	connect(srvSocketDev,&ARpcOutsideDevice::rawMessage,this,&ARpcOutsideDeviceWrap::rawMessage);
}

bool ARpcOutsideDeviceWrap::writeMsg(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::funcCallMsg)
		return srvSock->writeMsg("exec_command",QByteArrayList()<<deviceIdOrName<<"0"<<m.args);
	return false;
}

bool ARpcOutsideDeviceWrap::isConnected()
{
	return srvSock->isConnected();
}
