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
#include "Commands/TtyCommands.h"
#include "Commands/ListSensorsCommand.h"
#include "Commands/ExecDeviceCommandCommand.h"
#include "Commands/StoragesCommands.h"
#include "Commands/BindSensorCommand.h"
#include "Commands/ListControlsCommand.h"
#include "Commands/ListIdentifiedCommand.h"
#include "SysLogWrapper.h"
#include <QDebug>

//TODO identified_devices command
//TODO commands to get commands description

IotProxyCommandProcessor::IotProxyCommandProcessor(ARpcOutsideDevice *d,QObject *parent)
	:QObject(parent)
{
	dev=d;
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IotProxyCommandProcessor::onRawMessage,Qt::DirectConnection);

	addCommand(new TtyCommands(dev));
	addCommand(new ListSensorsCommand(dev));
	addCommand(new ExecDeviceCommandCommand(dev));
	addCommand(new StoragesCommands(dev));
	addCommand(new BindSensorCommand(dev));
	addCommand(new ListControlsCommand(dev));
	addCommand(new ListIdentifiedCommand(dev));
}

IotProxyCommandProcessor::~IotProxyCommandProcessor()
{
	commandProcs.clear();
	for(ICommand *c:commands)delete c;
}

void IotProxyCommandProcessor::onRawMessage(const ARpcMessage &m)
{
	qDebug()<<"Command from client: "<<m.title<<"; "<<m.args.join("|");
	if(commandProcs.contains(m.title))
	{
		ICommand *c=commandProcs[m.title];
		if(c->processCommand(m))
			dev->writeMsg(ARpcConfig::funcAnswerOkMsg);
		else dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QStringList()<<c->lastError());
	}
	else dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QStringList()<<"Unknown command: "<<m.title);
}

void IotProxyCommandProcessor::addCommand(ICommand *c)
{
	commands.append(c);
	QStringList cmds=c->acceptedCommands();
	for(QString &cmd:cmds)
		commandProcs[cmd]=c;
}
