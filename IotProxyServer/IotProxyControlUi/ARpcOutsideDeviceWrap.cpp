#include "ARpcOutsideDeviceWrap.h"
#include <QEventLoop>

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

void ARpcOutsideDeviceWrap::queryDeviceId()
{
	QByteArrayList retVal;
	if(execSrvCommand("device_id",QByteArrayList()<<deviceIdOrName,retVal)&&!retVal.isEmpty())
		deviceId=QUuid(retVal[0]);
}

bool ARpcOutsideDeviceWrap::listControls(ARpcControlsGroup &controls)
{
	QByteArrayList retVal;
	if(!execSrvCommand("list_controls",QByteArrayList()<<deviceIdOrName,retVal)||retVal.isEmpty())
		return false;
	if(retVal[0].startsWith("{"))
		return ARpcControlsGroup::parseJsonDescription(retVal[0],controls);
	else return ARpcControlsGroup::parseXmlDescription(retVal[0],controls);
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
		emit rawMessage(mm);
	}
	else if(m.title==ARpcConfig::stateChangedMsg)
	{
		if(m.args.isEmpty()||m.args[0]!=deviceId.toByteArray())return;
		mm.args.removeAt(0);
		emit rawMessage(mm);
	}
}

bool ARpcOutsideDeviceWrap::execSrvCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	if(!srvSock->isConnected())
	{
		retVal.append("not connected to remote server");
		return false;
	}
	QByteArray callId=QByteArray::number(qrand());
	QEventLoop loop;
	QTimer tmr;
	tmr.setSingleShot(true);
	tmr.setInterval(5000);
	connect(&tmr,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(srvSock,&ARpcOutsideDevice::disconnected,&loop,&QEventLoop::quit);
	bool done=false;
	auto conn=connect(srvSock,&ARpcOutsideDevice::rawMessage,
		[this,&loop,&done,&retVal,&callId](const ARpcMessage &m)->void
	{
		if(m.title==ARpcConfig::funcAnswerOkMsg&&!m.args.isEmpty()&&m.args[0]==callId)
		{
			done=true;
			retVal=m.args.mid(1);
			loop.quit();
		}
		else if(m.title==ARpcConfig::funcAnswerErrMsg&&!m.args.isEmpty()&&m.args[0]==callId)
		{
			retVal=m.args.mid(1);
			loop.quit();
		}
//		else if(m.title==ARpcServerConfig::srvCmdDataMsg&&!m.args.isEmpty()&&m.args[0]==callId)
//		{
//			if(!onCmData(m.args.mid(1)))
//				loop.quit();
//		}
	});
	srvSock->writeMsg(ARpcMessage(cmd,QByteArrayList()<<callId<<args));
	tmr.start();
	if(!done)loop.exec(QEventLoop::ExcludeUserInputEvents);
	tmr.stop();
	disconnect(conn);
	return done;
}
