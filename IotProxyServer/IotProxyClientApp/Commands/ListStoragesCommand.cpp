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
		QDebug(StdQFile::inst().stdout())<<"Storage:\n\tdevice id="<<m.args[0]<<"\n\tdevice name="<<m.args[1]<<
			"\n\tsensor name="<<m.args[2]<<"\n\tstorage type="<<m.args[3]<<"\n\ttimestamp transformation rule="<<
			m.args[4]<<"\n\n";
	}
}
