#include "ICommand.h"

ICommand::ICommand(ARpcOutsideDevice *d)
{
	clientDev=d;
}

const QString& ICommand::lastError()
{
	return lastErrorStr;
}
