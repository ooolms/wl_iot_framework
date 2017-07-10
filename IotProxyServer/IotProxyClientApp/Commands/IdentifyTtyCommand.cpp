#include "IdentifyTtyCommand.h"

IdentifyTtyCommand::IdentifyTtyCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent)
	:IClientCommand(p,d,parent)
{
}

bool IdentifyTtyCommand::evalCommand()
{
	return dev->writeMsg(ARpcMessage(IClientCommand::identifyTtyCommand));
}
