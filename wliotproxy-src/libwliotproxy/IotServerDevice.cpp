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

#include "IotServerDevice.h"
#include "wliot/WLIOTProtocolDefs.h"
#include <QEvent>
#include <QCoreApplication>

IotServerDevice::IotServerDevice(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,
	const QByteArray &name,const QUuid &typeId,QObject *parent)
	:RealDevice(parent)
{
	srvConn=conn;
	commands=cmds;
	devTypeId=typeId;
	devId=id;
	devName=name;
	onConnected();
}

IotServerDevice::~IotServerDevice()
{
}

void IotServerDevice::stateChangedFromServer(const QByteArrayList &args)
{
	onNewMessage(Message(WLIOTProtocolDefs::stateChangedMsg,args));
}

void IotServerDevice::setDisconnected()
{
	onDisconnected();
}

bool IotServerDevice::writeMsgToDevice(const Message &m)
{
	if(!isConnected())return false;
	qApp->postEvent(this,new MessageEvent(m));
	return true;
}

bool IotServerDevice::event(QEvent *event)
{
	if(event->type()==MessageEvent::type)
	{
		writeMessageToDeviceFromQueue(((MessageEvent*)event)->msg());
		return true;
	}
	else return QObject::event(event);
}

void IotServerDevice::writeMessageToDeviceFromQueue(const Message &m)
{
	//TODO identify_hub and hubs support (maybe other class for hubs?)
	if(!isConnected())return;
	if(m.title==WLIOTProtocolDefs::identifyMsg)
		onNewMessage(Message(WLIOTProtocolDefs::deviceInfoMsg,
			QByteArrayList()<<devId.toByteArray()<<devName<<devTypeId.toByteArray()));
	else if(m.title==WLIOTProtocolDefs::funcCallMsg)
	{
		if(m.args.count()<2)
		{
			onNewMessage(Message(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"invalid arguments"));
			return;
		}
		QByteArray callId=m.args[0];
		QByteArrayList retVal;
		bool wasDestroyed=false;
		connect(this,&IotServerDevice::destroyed,[&wasDestroyed](){wasDestroyed=true;});
		bool ok=commands->devices()->execDeviceCommand(devId.toByteArray(),m.args[1],m.args.mid(2),retVal);
		if(!wasDestroyed)
		{
			if(ok)
				onNewMessage(Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<retVal));
			else onNewMessage(Message(WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<retVal));
		}
	}
	else if(m.title==WLIOTProtocolDefs::devSyncMsg)
		onNewMessage(Message(WLIOTProtocolDefs::devSyncrMsg));
}
