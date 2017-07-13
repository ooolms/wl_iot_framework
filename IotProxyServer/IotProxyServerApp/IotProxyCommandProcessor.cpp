#include "IotProxyCommandProcessor.h"
#include "Commands/TtyCommands.h"
#include "Commands/ListSensorsCommand.h"
#include "Commands/ExecDeviceCommandCommand.h"
#include "Commands/StoragesCommands.h"
#include "SysLogWrapper.h"
#include <QDebug>

IotProxyCommandProcessor::IotProxyCommandProcessor(ARpcOutsideDevice *d,QObject *parent)
	:QObject(parent)
{
	dev=d;
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IotProxyCommandProcessor::onRawMessage);

	addCommand(new TtyCommands(dev));
	addCommand(new ListSensorsCommand(dev));
	addCommand(new ExecDeviceCommandCommand(dev));
	addCommand(new StoragesCommands(dev));
}

IotProxyCommandProcessor::~IotProxyCommandProcessor()
{
	commandProcs.clear();
	for(ICommand *c:commands)delete c;
}

void IotProxyCommandProcessor::onRawMessage(const ARpcMessage &m)
{
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
