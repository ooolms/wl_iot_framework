#include "ARpcOutsideDeviceWrap.h"

ARpcOutsideDeviceWrap::ARpcOutsideDeviceWrap(
	const QString &devIdOrName,ARpcOutsideDevice *srvSocketDev,QObject *parent)
	:ARpcDevice(parent)
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
		return srvSock->writeMsg("exec_command",QStringList()<<deviceIdOrName<<"0"<<m.args);
	return false;
}

bool ARpcOutsideDeviceWrap::isConnected()
{
	return srvSock->isConnected();
}
