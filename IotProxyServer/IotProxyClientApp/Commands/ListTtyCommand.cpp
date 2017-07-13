#include "ListTtyCommand.h"
#include "../StdQFile.h"
#include <QDebug>

ListTtyCommand::ListTtyCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
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
		QDebug d=StdQFile::inst().stdoutDebug();
		if(m.args.count()==6)d<<"Identified tty device:";
		else d<<"Tty device:";
		d<<"\n\tport: "<<m.args[0];
		d<<"\n\tserialNumber: "<<m.args[1];
		d<<"\n\tmanufacturer: "<<m.args[2];
		d<<"\n\tdescription: "<<m.args[3]<<"\n";
		if(m.args.count()==6)
		{
			d<<"\tuid="<<m.args[4];
			d<<"\n\tname="<<m.args[5]<<"\n";
		}
		d<<"\n";
	}
}
