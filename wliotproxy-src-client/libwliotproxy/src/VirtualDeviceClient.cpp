/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "wliot/client/VirtualDeviceClient.h"
#include "wliot/client/VirtualDeviceCallback.h"
#include "wliot/WLIOTProtocolDefs.h"

using namespace WLIOT;
using namespace WLIOTClient;

VirtualDeviceClient::VirtualDeviceClient(
	ServerConnection *conn,const QUuid &id,const QByteArray &name,const QUuid &typeId,
	const QList<SensorDef> &sensors,const ControlsGroup &controls,QObject *parent)
	:QObject(parent)
{
	srvConn=conn;
	devId=id;
	devTypeId=typeId;
	devName=name;
	callback=0;
	mSensors=sensors;
	mControls=controls;
}

void VirtualDeviceClient::onNewMessageFromServer(const Message &m)
{
	if(m.title==WLIOTProtocolDefs::identifyMsg)
	{
		srvConn->writeVDevMsg(devId,Message(
			WLIOTProtocolDefs::deviceInfoMsg,QByteArrayList()<<devId.toByteArray()<<devName<<devTypeId.toByteArray()));
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
	else if(m.title==WLIOTProtocolDefs::devSyncMsg)
	{
//		if(eventsCallback)
//			eventsCallback->onSyncMsg();
		srvConn->writeVDevMsg(devId,Message(WLIOTProtocolDefs::devSyncrMsg));
	}
	else if(m.title==WLIOTProtocolDefs::funcCallMsg)
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
			if(m.args[1]==WLIOTProtocolDefs::getSensorsCommand)
			{
				QByteArray data;
				SensorsParser::dumpToXml(data,mSensors);
				writeOk(callIdStr,QByteArrayList()<<data);
			}
			else if(m.args[1]==WLIOTProtocolDefs::getControlsCommand)
			{
				QByteArray data;
				ControlsParser::dumpToXml(data,mControls);
				writeOk(callIdStr,QByteArrayList()<<data);
			}
			else if(m.args[1]==WLIOTProtocolDefs::getStateCommand)
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

void VirtualDeviceClient::writeOk(const QByteArray &callId,const QByteArrayList &args)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<args));
}

void VirtualDeviceClient::writeErr(const QByteArray &callId, const QByteArrayList &args)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<args));
}

void VirtualDeviceClient::commandParamStateChanged(
	const QByteArray &cmd,quint32 paramIndex,const QByteArray &value)
{
	if(!mState.commandParams.contains(cmd))return;
	auto &pMap=mState.commandParams[cmd];
	if(!pMap.contains(paramIndex))return;
	pMap[paramIndex]=value;
	srvConn->writeVDevMsg(devId,Message(
		WLIOTProtocolDefs::stateChangedMsg,QByteArrayList()<<cmd<<QByteArray::number(paramIndex)<<value));
}

void VirtualDeviceClient::additionalStateChanged(const QByteArray &paramName,const QByteArray &value)
{
	if(!mState.additionalAttributes.contains(paramName))return;
	mState.additionalAttributes[paramName]=value;
	srvConn->writeVDevMsg(devId,Message(
		WLIOTProtocolDefs::stateChangedMsg,QByteArrayList()<<"#"<<paramName<<value));
}

void VirtualDeviceClient::sendVDevMeasurement(const QByteArray &sensor,const QByteArrayList &args)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTProtocolDefs::measurementMsg,QByteArrayList()<<sensor<<args));
}

void VirtualDeviceClient::sendVDevMeasurementB(const QByteArray &sensor, const SensorValue &val)
{
	srvConn->writeVDevMsg(devId,Message(
		WLIOTProtocolDefs::measurementBMsg,QByteArrayList()<<sensor<<val.dumpToBinary()));
}

void VirtualDeviceClient::sendVDevMeasurementB(const QByteArray &sensor,const QByteArray &val)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTProtocolDefs::measurementBMsg,QByteArrayList()<<sensor<<val));
}

void VirtualDeviceClient::writeInfo(const QByteArrayList &args)
{
	srvConn->writeVDevMsg(devId,Message(WLIOTProtocolDefs::infoMsg,args));
}

const DeviceState& VirtualDeviceClient::state()const
{
	return mState;
}
