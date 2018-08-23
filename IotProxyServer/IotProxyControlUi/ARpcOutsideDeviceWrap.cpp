#include "ARpcOutsideDeviceWrap.h"

ARpcOutsideDeviceWrap::ARpcOutsideDeviceWrap(
	const QByteArray &devIdOrName,ARpcOutsideDevice *srvSocketDev,QObject *parent)
	:ARpcRealDevice(parent)
{
	deviceIdOrName=devIdOrName;
	srvSock=srvSocketDev;
	connect(srvSocketDev,&ARpcOutsideDevice::connected,this,&ARpcOutsideDeviceWrap::connected);
	connect(srvSocketDev,&ARpcOutsideDevice::disconnected,this,&ARpcOutsideDeviceWrap::disconnected);
	connect(srvSocketDev,&ARpcOutsideDevice::rawMessage,this,&ARpcOutsideDeviceWrap::onRawMessage);
}

bool ARpcOutsideDeviceWrap::writeMsg(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::funcCallMsg)
	{
		if(m.args.isEmpty())return false;
		QByteArray callIdFromUi=m.args[0];
		QByteArray callIdToServer=QByteArray::number(qrand());
		callIdsMap[callIdToServer]=callIdFromUi;
		return srvSock->writeMsg("exec_command",QByteArrayList()<<callIdToServer<<deviceIdOrName<<m.args.mid(1));
	}
	return false;
}

bool ARpcOutsideDeviceWrap::isConnected()
{
	return srvSock->isConnected();
}

void ARpcOutsideDeviceWrap::onRawMessage(const ARpcMessage &m)
{
	ARpcMessage mm=m;
	if(m.title==ARpcConfig::funcAnswerOkMsg||m.title==ARpcConfig::funcAnswerErrMsg)
	{
		if(m.args.isEmpty())return;
		QByteArray callIdToServer=m.args[0];
		if(!callIdsMap.contains(callIdToServer))return;
		mm.args[0]=callIdsMap.take(callIdToServer);
	}
	emit rawMessage(mm);
}
