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

#include "FakeDeviceBackend.h"
#include "wliot/WLIOTProtocolDefs.h"
#include <QDebug>
#include <QThread>

using namespace WLIOTUi;
using namespace WLIOT;

FakeDeviceBackend::FakeDeviceBackend(QObject *parent)
	:IHighLevelDeviceBackend(parent)
{
	devId=QUuid::createUuid();
	mAddr="addr:"+devId.toString();
}

bool FakeDeviceBackend::isConnected()const
{
	return true;
}

void FakeDeviceBackend::forceDisconnect()
{
}

bool FakeDeviceBackend::writeMessageToDevice(const WLIOT::Message &m)
{
	if(m.title==WLIOTProtocolDefs::identifyMsg)
	{
		emit logMsg("Identify request");
		emit newMessageFromDevice(
			Message(WLIOTProtocolDefs::deviceInfoMsg,QByteArrayList()<<devId.toByteArray()<<"Test device"));
	}
	else if(m.title==WLIOTProtocolDefs::devSyncMsg)
		emit newMessageFromDevice(Message(WLIOTProtocolDefs::devSyncrMsg));
	else if(m.title==WLIOTProtocolDefs::funcCallMsg&&!m.args.isEmpty())
	{
		if(m.args.count()<1)
			return true;
		if(m.args.count()<2)
			emit newMessageFromDevice(Message(
				WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<m.args[0]<<"no command"));
		QByteArray callId=m.args[0];
		QByteArray func=m.args[1];
		QByteArrayList args=m.args.mid(2);
		emit logMsg("Function called: "+func+"; args: "+args.join("|"));
		if(func==WLIOTProtocolDefs::getControlsCommand)
		{
			QByteArray data;
			ControlsParser::dumpToXml(data,mControls);
			emit newMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<data));
		}
		else if(func==WLIOTProtocolDefs::getSensorsCommand)
		{
			QByteArray data;
			SensorsParser::dumpToXml(data,mSensors);
			emit newMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<data));
		}
		else if(func==WLIOTProtocolDefs::getStateCommand)
			emit newMessageFromDevice(
				Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<mState.dumpToMsgArgs()));
		else emit newMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId));
	}
	return true;
}

QByteArray FakeDeviceBackend::backendType()const
{
	return "fake";
}

QString FakeDeviceBackend::hwAddress()const
{
	return mAddr;
}
