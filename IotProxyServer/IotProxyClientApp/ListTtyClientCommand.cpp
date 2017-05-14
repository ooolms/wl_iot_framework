#include "ListTtyClientCommand.h"
#include "CommandNames.h"
#include <QDebug>

ListTtyClientCommand::ListTtyClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent)
	:IClientCommand(p,d,parent)
{
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&ListTtyClientCommand::onRawMessage);
}

bool ListTtyClientCommand::evalCommand()
{
	return dev->writeMsg(CommandNames::listTtyCommand);
}

void ListTtyClientCommand::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::srvCmdDataMsg)
	{
		if(m.args.count()<4)return;
		qDebug()<<"Tty device: port="<<m.args[0]<<"; serialNumber="<<m.args[1]<<"; manufacturer="<<m.args[2]<<
			"description="<<m.args[3];
		if(m.args.count()==6)
			qDebug()<<"\tIdentified device: uid="<<m.args[4]<<"; name="<<m.args[5];
	}
}
