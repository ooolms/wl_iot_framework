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
		StdQFile::inst().stdoutDebug()<<"Device identified\n\tId: "<<m.args[0]<<"\n\tname: "<<m.args[1]<<"\n";
	}
}
