#include "RemoveSensorCommand.h"
#include "../StdQFile.h"
#include <QDebug>

RemoveSensorCommand::RemoveSensorCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool RemoveSensorCommand::evalCommand()
{
	if(parser.getArgs().count()!=2)
	{
		//TODO help
		return false;
	}
	return dev->writeMsg(IClientCommand::removeSensorCommand,parser.getArgs());
}
