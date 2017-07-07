#ifndef EXECDEVICECOMMANDCOMMAND_H
#define EXECDEVICECOMMANDCOMMAND_H

#include "ICommand.h"

class ExecDeviceCommandCommand
	:public ICommand
{
public:
	explicit ExecDeviceCommandCommand(ARpcOutsideDevice *d);

public:
	virtual bool processCommand(const ARpcMessage &m)override;
	virtual QStringList acceptedCommands()override;
};

#endif // EXECDEVICECOMMANDCOMMAND_H
