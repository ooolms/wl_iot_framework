#ifndef TTYCOMMANDS_H
#define TTYCOMMANDS_H

#include "ICommand.h"

class TtyCommands
	:public ICommand
{
public:
	explicit TtyCommands(ARpcOutsideDevice *d);
	virtual bool processCommand(const ARpcMessage &m)override;
};

#endif // TTYCOMMANDS_H
