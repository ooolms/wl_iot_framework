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
#include "ARpcBase/ARpcConfig.h"

IotServerDevice::IotServerDevice(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,
	const QByteArray &name,const QByteArray &type,QObject *parent)
	:ARpcRealDevice(parent)
{
	srvConn=conn;
	commands=cmds;
	deviceConnectedFlag=false;
	devType=type;
	devId=id;
	devName=name;
	connect(srvConn,&IotServerConnection::connected,this,&IotServerDevice::onServerConnectionChanged);
	connect(srvConn,&IotServerConnection::disconnected,this,&IotServerDevice::onServerConnectionChanged);
}

void IotServerDevice::setDeviceConnected(bool c)
{
	if(deviceConnectedFlag==c)return;
	deviceConnectedFlag=c;
	if(!srvConn->isConnected())return;
	if(deviceConnectedFlag)
		emit connected();
	else emit disconnected();
}

void IotServerDevice::onServerConnectionChanged()
{
	if(!deviceConnectedFlag)return;
	bool serverConnectedFlag=srvConn->isConnected();
	if(serverConnectedFlag)
		emit connected();
	else emit disconnected();
}

bool IotServerDevice::writeMsg(const ARpcMessage &m)
{
	if(!isConnected())return false;
	messagesToDevice.append(m);
	QMetaObject::invokeMethod(this,"processMessages",Qt::QueuedConnection);
	return true;
}

bool IotServerDevice::isConnected()
{
	return deviceConnectedFlag&&srvConn->isConnected();
}

void IotServerDevice::processMessages()
{
	while(!messagesToDevice.isEmpty())
		processMessage(messagesToDevice.takeFirst());
}

void IotServerDevice::processMessage(const ARpcMessage &m)
{
	//TODO identify_hub and hubs support (maybe other class for hubs?)
	if(!isConnected())return;
	if(m.title==ARpcConfig::identifyMsg)
		emit newMessage(ARpcMessage(ARpcConfig::deviceInfoMsg,QByteArrayList()<<devId.toByteArray()<<devName));
	else if(m.title==ARpcConfig::funcCallMsg)
	{
		if(m.args.count()<2)
		{
			emit newMessage(ARpcMessage(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<"invalid arguments"));
			return;
		}
		QByteArray callId=m.args[0];
		QByteArrayList retVal;
		if(commands->devices()->execDeviceCommand(devId.toByteArray(),m.args[1],m.args.mid(2),retVal))
			emit newMessage(ARpcMessage(ARpcConfig::funcAnswerOkMsg,QByteArrayList()<<callId<<retVal));
		else emit newMessage(ARpcMessage(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<retVal));
	}
}
