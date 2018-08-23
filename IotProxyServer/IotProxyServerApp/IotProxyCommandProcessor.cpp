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

#include "IotProxyCommandProcessor.h"
#include "ARpcLocalStorage/ARpcISensorStorage.h"
#include "Commands/BindStorageCommand.h"
#include "Commands/DevicesConfigCommand.h"
#include "Commands/ExecDeviceCommandCommand.h"
#include "Commands/GetSamplesCommand.h"
#include "Commands/IdentifyCommand.h"
#include "Commands/IdentifyTcpCommand.h"
#include "Commands/JSControlCommand.h"
#include "Commands/ListControlsCommand.h"
#include "Commands/ListIdentifiedCommand.h"
#include "Commands/ListSensorsCommand.h"
#include "Commands/RegisterVirtualDeviceCommand.h"
#include "Commands/SessionStorageCommands.h"
#include "Commands/StoragesCommands.h"
#include "Commands/SubscribeCommand.h"
#include "Commands/TtyCommands.h"
#include "Commands/VdevMeasCommand.h"
#include "SysLogWrapper.h"
#include "IotProxyConfig.h"
#include "ARpcBase/ARpcServerConfig.h"
#include "IotProxyConfig.h"
#include "IotProxyInstance.h"
#include <QDebug>

IotProxyCommandProcessor::IotProxyCommandProcessor(ARpcOutsideDevice *d,bool needAuth,QObject *parent)
	:QObject(parent)
{
	inWork=false;
	needDeleteThis=false;
	dev=d;
	dev->setParent(this);
	ifNeedAuth=needAuth;
	authentificated=false;
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IotProxyCommandProcessor::onRawMessage,Qt::DirectConnection);
	connect(IotProxyInstance::inst().devices(),&IotProxyDevices::deviceIdentified,
		this,&IotProxyCommandProcessor::onDeviceIdentified,Qt::QueuedConnection);
	connect(IotProxyInstance::inst().devices(),&IotProxyDevices::deviceDisconnected,
		this,&IotProxyCommandProcessor::onDeviceLost,Qt::QueuedConnection);
	connect(IotProxyInstance::inst().sensorsStorage(),&ARpcLocalDatabase::storageCreated,
		this,&IotProxyCommandProcessor::onStorageCreated,Qt::QueuedConnection);
	connect(IotProxyInstance::inst().sensorsStorage(),&ARpcLocalDatabase::storageRemoved,
		this,&IotProxyCommandProcessor::onStorageRemoved,Qt::QueuedConnection);

	addCommand(new BindStorageCommand(dev,this));
	addCommand(new DevicesConfigCommand(dev,this));
	addCommand(new ExecDeviceCommandCommand(dev,this));
	addCommand(new GetSamplesCommand(dev,this));
	addCommand(new IdentifyCommand(dev,this));
	addCommand(new IdentifyTcpCommand(dev,this));
	addCommand(new JSControlCommand(dev,this));
	addCommand(new ListControlsCommand(dev,this));
	addCommand(new ListIdentifiedCommand(dev,this));
	addCommand(new ListSensorsCommand(dev,this));
	addCommand(new RegisterVirtualDeviceCommand(dev,this));
	addCommand(new SessionStorageCommands(dev,this));
	addCommand(new StoragesCommands(dev,this));
	addCommand(new SubscribeCommand(dev,this));
	addCommand(new TtyCommands(dev,this));
	addCommand(new VdevMeasCommand(dev,this));
}

IotProxyCommandProcessor::~IotProxyCommandProcessor()
{
	commandProcs.clear();
	for(ICommand *c:commands)
		delete c;
}

void IotProxyCommandProcessor::scheduleDelete()
{
	if(!inWork)delete this;
	else needDeleteThis=true;
}

void IotProxyCommandProcessor::onNewValueWritten(const ARpcSensorValue *value)
{
	if(ifNeedAuth&&!authentificated)return;
	inWork=true;
	ARpcISensorStorage *stor=(ARpcISensorStorage*)sender();
	dev->writeMsg(ARpcConfig::measurementMsg,
		QByteArrayList()<<stor->deviceId().toByteArray()<<stor->sensor().name<<value->dumpToMsgArgs());
	inWork=false;
	if(needDeleteThis)
		delete this;
}

void IotProxyCommandProcessor::onRawMessage(const ARpcMessage &m)
{
	inWork=true;
	if(m.title==ARpcConfig::identifyMsg)
		dev->writeMsg(ARpcConfig::funcAnswerOkMsg,
			QByteArrayList()<<IotProxyConfig::serverId.toByteArray()<<IotProxyConfig::serverName.toUtf8());
	else if(m.title==ARpcServerConfig::authentificateSrvMsg)
	{
		if(m.args.count()<2)
		{
			qDebug()<<"authentification failed";
			dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<"authentification failed");
		}
		QByteArray callId=m.args[0];
		qDebug()<<"authentification required";
		if(!IotProxyConfig::networkAccessKey.isEmpty()&&m.args.count()==2&&m.args[1]==IotProxyConfig::networkAccessKey)
		{
			authentificated=true;
			qDebug()<<"authentification done";
			dev->writeMsg(ARpcConfig::funcAnswerOkMsg,QByteArrayList()<<callId<<"authentification done");
		}
		else
		{
			qDebug()<<"authentification failed";
			dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentification failed");
		}
		inWork=false;
		if(needDeleteThis)
			delete this;
		return;
	}
	qDebug()<<"command from client: "<<m.title<<"; "<<m.args.join("|");
	if(ifNeedAuth&&!authentificated)
		dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<"authentification required");
	else
	{
		if(m.args.count()<1)
		{
			qDebug()<<"invalid command";
			dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<"invalid command");
			inWork=false;
			if(needDeleteThis)
				delete this;
			return;
		}
		QByteArray callId=m.args[0];
		QByteArrayList args=m.args.mid(1);
		if(commandProcs.contains(m.title))
		{
			ICommand *c=commandProcs[m.title];
			QByteArrayList retVal;
			if(c->processCommand(m.title,callId,args,retVal))
				dev->writeMsg(ARpcConfig::funcAnswerOkMsg,retVal);
			else
				dev->writeMsg(ARpcConfig::funcAnswerErrMsg,retVal);
		}
		else
			dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<"unknown command"<<m.title);
	}
	inWork=false;
	if(needDeleteThis)
		delete this;
}

void IotProxyCommandProcessor::onDeviceIdentified(QUuid id,QByteArray name)
{
	if(ifNeedAuth&&!authentificated)return;
	dev->writeMsg(ARpcServerConfig::notifyDeviceIdentifiedMsg,QByteArrayList()<<id.toByteArray()<<name);
}

void IotProxyCommandProcessor::onDeviceLost(QUuid id)
{
	if(ifNeedAuth&&!authentificated)return;
	dev->writeMsg(ARpcServerConfig::notifyDeviceLostMsg,QByteArrayList()<<id.toByteArray());
}

void IotProxyCommandProcessor::onStorageCreated(const DeviceStorageId &id)
{
	if(ifNeedAuth&&!authentificated)return;
	ARpcISensorStorage *st=IotProxyInstance::inst().sensorsStorage()->existingStorage(id);
	if(!st)return;
	dev->writeMsg(ARpcServerConfig::notifyStorageCreatedMsg,StoragesCommands::storageToMsgArguments(st));
}

void IotProxyCommandProcessor::onStorageRemoved(const DeviceStorageId &id)
{
	if(ifNeedAuth&&!authentificated)return;
	dev->writeMsg(ARpcServerConfig::notifyStorageRemovedMsg,QByteArrayList()<<id.deviceId.toByteArray()<<id.sensorName);
}

void IotProxyCommandProcessor::addCommand(ICommand *c)
{
	commands.append(c);
	QByteArrayList cmds=c->acceptedCommands();
	for(QByteArray &cmd:cmds)
		commandProcs[cmd]=c;
}
