#include "ListStoragesCommand.h"
#include "../StdQFile.h"
#include <QDebug>

ListStoragesCommand::ListStoragesCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&ListStoragesCommand::onRawMessage);
}

bool ListStoragesCommand::evalCommand()
{
	return dev->writeMsg(IClientCommand::listStoragesCommand);
}

void ListStoragesCommand::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::srvCmdDataMsg)
	{
		if(m.args.count()<5)return;
		QDebug d=StdQFile::inst().stdoutDebug();
		d<<"Storage:";
		d<<"\n\tdevice id: "<<m.args[0];
		d<<"\n\tdevice name: "<<m.args[1];
		d<<"\n\tsensor name: "<<m.args[2];
		d<<"\n\tstorage type: "<<m.args[3];
		d<<"\n\ttimestamp transformation rule: "<<m.args[4]<<"\n";
	}
}
