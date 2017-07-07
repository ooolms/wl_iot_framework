#ifndef TTYCOMMANDS_H
#define TTYCOMMANDS_H

#include "ICommand.h"

class TtyCommands
	:public ICommand
{
public:
	explicit TtyCommands(ARpcOutsideDevice *d);
	virtual bool processCommand(const ARpcMessage &m)override;
	virtual QStringList acceptedCommands()override;

private:
	bool listTtyDevices(const ARpcMessage &m);
	bool indentifyTtyDevice(const ARpcMessage &m);
};

#endif // TTYCOMMANDS_H
