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
#include "Commands/BindSensorCommand.h"
#include "Commands/DevicesConfigCommand.h"
#include "Commands/ExecDeviceCommandCommand.h"
#include "Commands/GetSamplesCommand.h"
#include "Commands/JSControlCommand.h"
#include "Commands/ListControlsCommand.h"
#include "Commands/ListIdentifiedCommand.h"
#include "Commands/ListSensorsCommand.h"
#include "Commands/RegisterVirtualDeviceCommand.h"
#include "Commands/StoragesCommands.h"
#include "Commands/SubscribeCommand.h"
#include "Commands/TtyCommands.h"
#include "Commands/VdevMeasCommand.h"
#include "Commands/IdentifyCommand.h"
#include "SysLogWrapper.h"
#include "IotProxyConfig.h"
#include "ARpcBase/ARpcServerConfig.h"
#include "IotProxyConfig.h"
#include <QDebug>

//TODO identified_devices command
//TODO commands to get commands description

IotProxyCommandProcessor::IotProxyCommandProcessor(ARpcOutsideDevice *d,bool needAuth,QObject *parent)
	:QObject(parent)
{
	dev=d;
	ifNeedAuth=needAuth;
	authentificated=false;
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IotProxyCommandProcessor::onRawMessage,Qt::DirectConnection);

	addCommand(new BindSensorCommand(dev,this));
	addCommand(new DevicesConfigCommand(dev,this));
	addCommand(new ExecDeviceCommandCommand(dev,this));
	addCommand(new GetSamplesCommand(dev,this));
	addCommand(new JSControlCommand(dev,this));
	addCommand(new ListControlsCommand(dev,this));
	addCommand(new ListIdentifiedCommand(dev,this));
	addCommand(new ListSensorsCommand(dev,this));
	addCommand(new RegisterVirtualDeviceCommand(dev,this));
	addCommand(new StoragesCommands(dev,this));
	addCommand(new SubscribeCommand(dev,this));
	addCommand(new TtyCommands(dev,this));
	addCommand(new VdevMeasCommand(dev,this));
	addCommand(new IdentifyCommand(dev,this));
}

IotProxyCommandProcessor::~IotProxyCommandProcessor()
{
	commandProcs.clear();
	for(ICommand *c:commands)
		delete c;
}

void IotProxyCommandProcessor::onNewValueWritten(const ARpcISensorValue *value)
{
	ARpcISensorStorage *stor=(ARpcISensorStorage*)sender();
	dev->writeMsg(ARpcConfig::measurementMsg,
		QStringList()<<stor->deviceId().toString()<<stor->sensor().name<<value->dump());
}

void IotProxyCommandProcessor::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::identifyMsg)
		dev->writeMsg(ARpcConfig::funcAnswerOkMsg,
			QStringList()<<IotProxyConfig::serverId.toString()<<
				IotProxyConfig::serverName);
	else if(m.title==ARpcServerConfig::authentificateSrvMsg)
	{
		qDebug()<<"authentification required";
		if(!IotProxyConfig::networkAccessKey.isEmpty()&&m.args.count()>=1&&m.args[0]==IotProxyConfig::networkAccessKey)
		{
			authentificated=true;
			dev->writeMsg(ARpcConfig::funcAnswerOkMsg,QStringList()<<"authentification done");
		}
		else
			dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QStringList()<<"authentification failed");
		return;
	}
	qDebug()<<"command from client: "<<m.title<<"; "<<m.args.join("|");
	if(ifNeedAuth&&!authentificated)
		dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QStringList()<<"authentification required");
	else
	{
		if(commandProcs.contains(m.title))
		{
			ICommand *c=commandProcs[m.title];
			QStringList retVal;
			if(c->processCommand(m,retVal))
				dev->writeMsg(ARpcConfig::funcAnswerOkMsg,retVal);
			else
				dev->writeMsg(ARpcConfig::funcAnswerErrMsg,retVal);
		}
		else
			dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QStringList()<<"unknown command"<<m.title);
	}
}

void IotProxyCommandProcessor::addCommand(ICommand *c)
{
	commands.append(c);
	QStringList cmds=c->acceptedCommands();
	for(QString &cmd:cmds)
		commandProcs[cmd]=c;
}
