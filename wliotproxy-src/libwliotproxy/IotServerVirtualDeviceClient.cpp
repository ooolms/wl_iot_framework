#include "IotServerVirtualDeviceClient.h"
#include "IotServerVirtualDeviceCallback.h"
#include "wliot/WLIOTConfig.h"

IotServerVirtualDeviceClient::IotServerVirtualDeviceClient(IotServerConnection *conn,const QUuid &id,
	const QByteArray &name,const QList<SensorDef> &sensors,const ControlsGroup &controls, QObject *parent)
	:QObject(parent)
{
	srvConn=conn;
	devId=id;
	devName=name;
	callback=0;
	mSensors=sensors;
	mControls=controls;
	prepareStateFromControls(controls);
}

void IotServerVirtualDeviceClient::setDevEventsCallback(IotServerVirtualDeviceCallback *cb)
{
	callback=cb;
}

void IotServerVirtualDeviceClient::setupAdditionalStateAttributes(const QByteArrayList &names)
{
	mState.additionalAttributes.clear();
	for(const auto &n:names)
		mState.additionalAttributes[n]=QByteArray();
}

void IotServerVirtualDeviceClient::onNewMessageFromServer(const Message &m)
{
	if(m.title==WLIOTConfig::identifyMsg)
	{
		srvConn->writeVDevMsg(devId,Message(WLIOTConfig::deviceInfoMsg,QByteArrayList()<<devId.toByteArray()<<devName));
	}
	/*else if(strcmp(msg,"identify_hub")==0)
	{
		if(!hubMsgCallback)
		{
			writeErr("not a hub device");
			return;
		}
		const char *msg="identify";
		hubMsgCallback->processMsg(bCastMsg,&msg,1);
		writeOk();
	}*/
//	else if(strcmp(msg,"queryversion")==0)
//		mWriter->writeMsg("version","simple_v1.1");
	else if(m.title==WLIOTConfig::devSyncMsg)
	{
//		if(eventsCallback)
//			eventsCallback->onSyncMsg();
		srvConn->writeVDevMsg(devId,Message(WLIOTConfig::devSyncrMsg));
	}
	else if(m.title==WLIOTConfig::funcCallMsg)
	{
		QByteArray callIdStr;
		if(m.args.count()<2||m.args[0].size()==0||m.args[1].size()==0)
		{
			writeErr("",QByteArrayList()<<"No command or call id");
			return;
		}
		callIdStr=m.args[0];
		if(m.args[1][0]=='#')
		{
			if(m.args[1]==WLIOTConfig::getSensorsCommand)
			{
				QByteArray data;
				SensorDef::dumpToXml(data,mSensors);
				writeOk(callIdStr,QByteArrayList()<<data);
			}
			else if(m.args[1]==WLIOTConfig::getControlsCommand)
			{
				QByteArray data;
				ControlsGroup::dumpToXml(data,mControls);
				writeOk(callIdStr,QByteArrayList()<<data);
			}
			else if(m.args[1]==WLIOTConfig::getStateCommand)
				writeOk(callIdStr,mState.dumpToMsgArgs());
			else writeErr(callIdStr,QByteArrayList()<<"bad system command");
		}
		else
		{
			QByteArrayList retVal;
			bool ok=false;
			if(callback)
				ok=callback->processCommand(m.args[1],m.args.mid(2),retVal);
			else retVal.append("unknown command");
			if(ok)writeOk(callIdStr,retVal);
			else writeErr(callIdStr,retVal);
		}
	}
}

void IotServerVirtualDeviceClient::writeOk(const QByteArray &callId,const QByteArrayList &args)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTConfig::funcAnswerOkMsg,QByteArrayList()<<callId<<args));
}

void IotServerVirtualDeviceClient::writeErr(const QByteArray &callId, const QByteArrayList &args)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<args));
}

void IotServerVirtualDeviceClient::prepareStateFromControls(const ControlsGroup &grp)
{
	for(int i=0;i<grp.elements.count();++i)
	{
		const ControlsGroup::Element &elem=grp.elements[i];
		if(elem.isGroup())
			prepareStateFromControls(*elem.group());
		else
		{
			const CommandControl *ctl=elem.control();
			auto &paramsMap=mState.commandParams[ctl->command];
			for(int i=0;i<ctl->params.count();++i)
				paramsMap[i+1]=QByteArray();
		}
	}
}

void IotServerVirtualDeviceClient::commandParamStateChanged(
	const QByteArray &cmd,int paramIndex,const QByteArray &value)
{
	if(!mState.commandParams.contains(cmd))return;
	auto &pMap=mState.commandParams[cmd];
	if(!pMap.contains(paramIndex))return;
	pMap[paramIndex]=value;
	srvConn->writeVDevMsg(devId,Message(
		WLIOTConfig::stateChangedMsg,QByteArrayList()<<cmd<<QByteArray::number(paramIndex)<<value));
}

void IotServerVirtualDeviceClient::additionalStateChanged(const QByteArray &paramName,const QByteArray &value)
{
	if(!mState.additionalAttributes.contains(paramName))return;
	mState.additionalAttributes[paramName]=value;
	srvConn->writeVDevMsg(devId,Message(
		WLIOTConfig::stateChangedMsg,QByteArrayList()<<"#"<<paramName<<value));
}

void IotServerVirtualDeviceClient::sendVDevMeasurement(const QByteArray &sensor,const QByteArrayList &args)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTConfig::measurementMsg,QByteArrayList()<<sensor<<args));
}

void IotServerVirtualDeviceClient::sendVDevMeasurementB(const QByteArray &sensor,const SensorValue &val)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTConfig::measurementBMsg,QByteArrayList()<<sensor<<val.dumpToBinary()));
}

void IotServerVirtualDeviceClient::writeInfo(const QByteArrayList &args)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTConfig::infoMsg,args));
}
