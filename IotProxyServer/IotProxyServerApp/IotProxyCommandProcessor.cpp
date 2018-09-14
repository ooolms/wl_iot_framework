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
#include "ARpcStorages/ARpcBaseFSSensorStorage.h"
#include "Commands/DevicesConfigCommand.h"
#include "Commands/DeviceIdCommand.h"
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
#include "Commands/AvailableDataExportServicesCommand.h"
#include "SysLogWrapper.h"
#include "IotProxyConfig.h"
#include "ARpcBase/ARpcServerConfig.h"
#include "IotProxyConfig.h"
#include "IotProxyInstance.h"
#include <QDebug>
#include <QEventLoop>

const QByteArray IotProxyCommandProcessor::vDevOkMsg="vdev_ok";
const QByteArray IotProxyCommandProcessor::vDevErrMsg="vdev_err";

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
	connect(IotProxyInstance::inst().devices(),&IotProxyDevices::deviceStateChanged,
		this,&IotProxyCommandProcessor::onDeviceStateChanged,Qt::QueuedConnection);
	connect(IotProxyInstance::inst().sensorsStorage(),&ARpcFSStoragesDatabase::storageCreated,
		this,&IotProxyCommandProcessor::onStorageCreated,Qt::QueuedConnection);
	connect(IotProxyInstance::inst().sensorsStorage(),&ARpcFSStoragesDatabase::storageRemoved,
		this,&IotProxyCommandProcessor::onStorageRemoved,Qt::QueuedConnection);

	addCommand(new DevicesConfigCommand(dev,this));
	addCommand(new DeviceIdCommand(dev,this));
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
	addCommand(new AvailableDataExportServicesCommand(dev,this));
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

void IotProxyCommandProcessor::registerVDevForCommandsProcessing(ARpcVirtualDevice *d)
{
	if(vDevs.contains(d))return;
	connect(d,&ARpcVirtualDevice::processDeviceCommand,this,&IotProxyCommandProcessor::onProcessCommandFromVDev);
	connect(d,&ARpcVirtualDevice::destroyed,[this,d]()
	{
		vDevs.removeOne(d);
	});
	vDevs.append(d);
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
	if(m.title==ARpcConfig::funcAnswerOkMsg||m.title==ARpcConfig::funcAnswerErrMsg||
		m.title==vDevOkMsg||m.title==vDevErrMsg||m.title==ARpcConfig::funcSynccMsg)
		return;
	inWork=true;
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
	if(m.title==ARpcConfig::identifyMsg)
		dev->writeMsg(ARpcConfig::funcAnswerOkMsg,
			QByteArrayList()<<callId<<IotProxyConfig::serverId.toByteArray()<<IotProxyConfig::serverName.toUtf8());
	else if(m.title==ARpcServerConfig::authentificateSrvMsg)
	{
		if(m.args.count()<2)
		{
			qDebug()<<"authentification failed";
			dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<"authentification failed");
		}
		qDebug()<<"authentification in process";
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
		dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QByteArrayList()<<callId<<"authentification required");
	else
	{
		QByteArray callId=m.args[0];
		QByteArrayList args=m.args.mid(1);
		if(commandProcs.contains(m.title))
		{
			ICommand *c=commandProcs[m.title];
			QByteArrayList retVal;
			bool ok=c->processCommand(m.title,callId,args,retVal);
			if(ok)
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

void IotProxyCommandProcessor::onDeviceStateChanged(QUuid id,QByteArrayList args)
{
	if(ifNeedAuth&&!authentificated)return;
	dev->writeMsg(ARpcConfig::stateChangedMsg,QByteArrayList()<<id.toByteArray()<<args);
}

void IotProxyCommandProcessor::onDeviceLost(QUuid id)
{
	if(ifNeedAuth&&!authentificated)return;
	dev->writeMsg(ARpcServerConfig::notifyDeviceLostMsg,QByteArrayList()<<id.toByteArray());
}

void IotProxyCommandProcessor::onStorageCreated(const ARpcStorageId &id)
{
	if(ifNeedAuth&&!authentificated)return;
	ARpcISensorStorage *st=IotProxyInstance::inst().sensorsStorage()->existingStorage(id);
	if(!st)return;
	dev->writeMsg(ARpcServerConfig::notifyStorageCreatedMsg,StoragesCommands::storageToMsgArguments(st));
}

void IotProxyCommandProcessor::onStorageRemoved(const ARpcStorageId &id)
{
	if(ifNeedAuth&&!authentificated)return;
	dev->writeMsg(ARpcServerConfig::notifyStorageRemovedMsg,QByteArrayList()<<id.deviceId.toByteArray()<<id.sensorName);
}

void IotProxyCommandProcessor::onProcessCommandFromVDev(
	const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal)
{
	ARpcVirtualDevice *vDev=(ARpcVirtualDevice*)sender();
	QByteArray callId=QByteArray::number(qrand());
	QTimer t;
	t.setInterval(ARpcConfig::synccCallWaitTime);
	t.setSingleShot(true);
	ok=false;
	bool done=false;
	QEventLoop loop;
	auto conn=connect(dev,&ARpcOutsideDevice::rawMessage,
		[this,&retVal,&ok,&t,&loop,&done,&callId,&vDev](const ARpcMessage &m)
	{
		if(m.args.isEmpty())return;
		if(m.title==vDevOkMsg&&m.args[0]==callId)
		{
			ok=true;
			done=true;
			retVal=m.args.mid(1);
			loop.quit();
		}
		else if(m.title==vDevErrMsg&&m.args[0]==callId)
		{
			done=true;
			retVal=m.args.mid(1);
			loop.quit();
		}
		else if(m.title==ARpcConfig::funcSynccMsg&&m.args[0]==callId)
		{
			vDev->writeMsgFromDevice(ARpcMessage(ARpcConfig::funcSynccMsg));
			t.stop();
			t.start();
		}
	});
	dev->writeMsg("vdev_command",QByteArrayList()<<callId<<cmd<<args);
	t.start();
	if(!done)loop.exec(QEventLoop::ExcludeUserInputEvents);
	disconnect(conn);
}

void IotProxyCommandProcessor::addCommand(ICommand *c)
{
	commands.append(c);
	QByteArrayList cmds=c->acceptedCommands();
	for(QByteArray &cmd:cmds)
		commandProcs[cmd]=c;
}
