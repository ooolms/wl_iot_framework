#include "ListTtyCommand.h"
#include <QDebug>

ListTtyCommand::ListTtyCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent)
	:IClientCommand(p,d,parent)
{
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&ListTtyCommand::onRawMessage);
}

bool ListTtyCommand::evalCommand()
{
	return dev->writeMsg(IClientCommand::listTtyCommand);
}

void ListTtyCommand::onRawMessage(const ARpcMessage &m)
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
