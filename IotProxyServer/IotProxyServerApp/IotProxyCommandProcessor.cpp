#include "IotProxyCommandProcessor.h"
#include "Commands/TtyCommands.h"
#include "Commands/ListSensorsCommand.h"
#include "SysLogWrapper.h"
#include <QDebug>

IotProxyCommandProcessor::IotProxyCommandProcessor(ARpcOutsideDevice *d,QObject *parent)
	:QObject(parent)
{
	dev=d;
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IotProxyCommandProcessor::onRawMessage);

	addCommand(new TtyCommands(dev));
	addCommand(new ListSensorsCommand(dev));
}

IotProxyCommandProcessor::~IotProxyCommandProcessor()
{
	commandProcs.clear();
	for(ICommand *c:commands)delete c;
}

void IotProxyCommandProcessor::onRawMessage(const ARpcMessage &m)
{
	qDebug()<<"raw message: "<<m.title<<"; "<<m.args;
	if(commandProcs.contains(m.title))
	{
		ICommand *c=commandProcs[m.title];
		if(c->processCommand(m))
			dev->writeMsg(ARpcConfig::funcAnswerOkMsg);
		else dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QStringList()<<c->lastError());
	}
	else dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QStringList()<<"Unknown command");
}

void IotProxyCommandProcessor::addCommand(ICommand *c)
{
	commands.append(c);
	QStringList cmds=c->acceptedCommands();
	for(QString &cmd:cmds)
		commandProcs[cmd]=c;
}
