#include "IotProxyCommandProcessor.h"
#include "Commands/TtyCommands.h"
#include "SysLogWrapper.h"
#include <QDebug>

IotProxyCommandProcessor::IotProxyCommandProcessor(ARpcOutsideDevice *d,QObject *parent)
	:QObject(parent)
{
	dev=d;
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&IotProxyCommandProcessor::onRawMessage);
	TtyCommands *ttyCmd=new TtyCommands(dev);
	commandProcs["list_tty"]=ttyCmd;
}

void IotProxyCommandProcessor::onRawMessage(const ARpcMessage &m)
{
	qDebug()<<"raw message: "<<m.title<<"; "<<m.args;
	if(commandProcs.contains(m.title))
	{
		if(commandProcs[m.title]->processCommand(m))
			dev->writeMsg(ARpcConfig::funcAnswerOkMsg);
		else dev->writeMsg(ARpcConfig::funcAnswerErrMsg);
	}
	else dev->writeMsg(ARpcConfig::funcAnswerErrMsg,QStringList()<<"Unknown command");
}
