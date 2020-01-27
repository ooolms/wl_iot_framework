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

#include "IotServerDeviceBackend.h"
#include "wliot/WLIOTProtocolDefs.h"
#include <QEvent>
#include <QCoreApplication>

IotServerDeviceBackend::IotServerDeviceBackend(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,
	const QByteArray &name,const QUuid &classId,const QByteArray &devType,const QByteArray &devPortName,
	QObject *parent)
	:IHighLevelDeviceBackend(parent)
{
	srvConn=conn;
	commands=cmds;
	devClassId=classId;
	devId=id;
	devName=name;
	mType=devType;
	mPortName=devPortName;
	mConnected=true;
}

IotServerDeviceBackend::~IotServerDeviceBackend()
{
}

void IotServerDeviceBackend::stateChangedFromServer(const QByteArrayList &args)
{
	emit newMessageFromDevice(Message(WLIOTProtocolDefs::stateChangedMsg,args));
}

void IotServerDeviceBackend::setDisconnected()
{
	mConnected=false;
	emit disconnected();
}

bool IotServerDeviceBackend::writeMessageToDevice(const Message &m)
{
	if(!isConnected())return false;
	qApp->postEvent(this,new MessageEvent(m));
	return true;
}

bool IotServerDeviceBackend::event(QEvent *event)
{
	if(event->type()==MessageEvent::type)
	{
		writeMessageToDeviceFromQueue(((MessageEvent*)event)->msg());
		return true;
	}
	else return QObject::event(event);
}

void IotServerDeviceBackend::writeMessageToDeviceFromQueue(const Message &m)
{
	//TODO identify_hub and hubs support (maybe other class for hubs?)
	if(!isConnected())return;
	if(m.title==WLIOTProtocolDefs::identifyMsg)
		emit newMessageFromDevice(Message(WLIOTProtocolDefs::deviceInfoMsg,
			QByteArrayList()<<devId.toByteArray()<<devName<<devClassId.toByteArray()));
	else if(m.title==WLIOTProtocolDefs::funcCallMsg)
	{
		if(m.args.count()<2)
		{
			emit newMessageFromDevice(Message(
				WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<"invalid arguments"));
			return;
		}
		QByteArray callId=m.args[0];
		QByteArrayList retVal;
		bool wasDestroyed=false;
		connect(this,&IotServerDeviceBackend::destroyed,[&wasDestroyed](){wasDestroyed=true;});
		bool ok=commands->devices()->execDeviceCommand(devId.toByteArray(),m.args[1],m.args.mid(2),retVal);
		if(!wasDestroyed)
		{
			if(ok)
				emit newMessageFromDevice(Message(WLIOTProtocolDefs::funcAnswerOkMsg,QByteArrayList()<<callId<<retVal));
			else emit newMessageFromDevice(Message(
				WLIOTProtocolDefs::funcAnswerErrMsg,QByteArrayList()<<callId<<retVal));
		}
	}
	else if(m.title==WLIOTProtocolDefs::devSyncMsg)
		emit newMessageFromDevice(Message(WLIOTProtocolDefs::devSyncrMsg));
}

bool IotServerDeviceBackend::isConnected()const
{
	return mConnected;
}

void IotServerDeviceBackend::forceDisconnect()
{
}

QByteArray IotServerDeviceBackend::type()const
{
	return mType;
}

QByteArray IotServerDeviceBackend::portOrAddress()const
{
	return mPortName;
}
