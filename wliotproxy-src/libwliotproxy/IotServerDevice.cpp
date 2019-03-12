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
#include "wliot/WLIOTConfig.h"

IotServerDevice::IotServerDevice(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,
	const QByteArray &name,const QByteArray &type,QObject *parent)
	:RealDevice(parent)
{
	srvConn=conn;
	commands=cmds;
	devType=type;
	devId=id;
	devName=name;
	onConnected();
}

void IotServerDevice::stateChangedFromServer(const QByteArrayList &args)
{
	onNewMessage(Message(WLIOTConfig::stateChangedMsg,args));
}

void IotServerDevice::setDisconnected()
{
	onDisconnected();
}

bool IotServerDevice::writeMsgToDevice(const Message &m)
{
	if(!isConnected())return false;
	messagesToDevice.append(m);
	QMetaObject::invokeMethod(this,"processMessages",Qt::QueuedConnection);
	return true;
}

void IotServerDevice::processMessagesToDevice()
{
	while(!messagesToDevice.isEmpty())
		writeMessageToDeviceFromQueue(messagesToDevice.takeFirst());
}

void IotServerDevice::writeMessageToDeviceFromQueue(const Message &m)
{
	//TODO identify_hub and hubs support (maybe other class for hubs?)
	if(!isConnected())return;
	if(m.title==WLIOTConfig::identifyMsg)
		onNewMessage(Message(WLIOTConfig::deviceInfoMsg,QByteArrayList()<<devId.toByteArray()<<devName));
	else if(m.title==WLIOTConfig::funcCallMsg)
	{
		if(m.args.count()<2)
		{
			onNewMessage(Message(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<"invalid arguments"));
			return;
		}
		QByteArray callId=m.args[0];
		QByteArrayList retVal;
		if(commands->devices()->execDeviceCommand(devId.toByteArray(),m.args[1],m.args.mid(2),retVal))
			onNewMessage(Message(WLIOTConfig::funcAnswerOkMsg,QByteArrayList()<<callId<<retVal));
		else onNewMessage(Message(WLIOTConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<retVal));
	}
	else if(m.title==WLIOTConfig::devSyncMsg)
		onNewMessage(Message(WLIOTConfig::devSyncrMsg));
}
