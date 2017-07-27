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
		if(m.args.count()<6)return;
		QDebug d=StdQFile::inst().stdoutDebug();
		if(m.args.count()==8)d<<"Identified tty device:";
		else d<<"Tty device:";
		d<<"\n\tport: "<<m.args[0];
		d<<"\n\tserialNumber: "<<m.args[1];
		d<<"\n\tmanufacturer: "<<m.args[2];
		d<<"\n\tusb vendor id (0 - not a usb device): "<<m.args[3];
		d<<"\n\tusb product id (0 - not a usb device): "<<m.args[4];
		d<<"\n\tusb product: "<<m.args[5]<<"\n";
		if(m.args.count()==8)
		{
			d<<"\tuid="<<m.args[6];
			d<<"\n\tname="<<m.args[7]<<"\n";
		}
		d<<"\n";
	}
}
