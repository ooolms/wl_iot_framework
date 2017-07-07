#ifndef LISTSENSORSCOMMAND_H
#define LISTSENSORSCOMMAND_H

#include "ICommand.h"

class ListSensorsCommand
	:public ICommand
{
public:
	explicit ListSensorsCommand(ARpcOutsideDevice *d);

public:
	virtual bool processCommand(const ARpcMessage &m)override;
	virtual QStringList acceptedCommands()override;
};

#endif // LISTSENSORSCOMMAND_H
