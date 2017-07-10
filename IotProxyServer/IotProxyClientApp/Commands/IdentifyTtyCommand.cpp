#include "IdentifyTtyCommand.h"
#include "../StdQFile.h"
#include <QDebug>

IdentifyTtyCommand::IdentifyTtyCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
	connect(d,&ARpcOutsideDevice::rawMessage,this,&IdentifyTtyCommand::onRawMessage);
}

bool IdentifyTtyCommand::evalCommand()
{
	if(parser.getArgs().count()!=1)
	{
		//TODO help
		return false;
	}
	return dev->writeMsg(IClientCommand::identifyTtyCommand,parser.getArgs());
}

void IdentifyTtyCommand::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::srvCmdDataMsg)
	{
		if(m.args.count()<2)return;
		QDebug(StdQFile::inst().stdout())<<"Device identified. Id: "<<m.args[0]<<"; name: "<<m.args[1]<<"\n";
	}
}
