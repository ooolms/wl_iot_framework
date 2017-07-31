#ifndef BINDSENSORCOMMAND_H
#define BINDSENSORCOMMAND_H

#include "../IClientCommand.h"

class BindSensorCommand
	:public IClientCommand
{
public:
	explicit BindSensorCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;
};

#endif // BINDSENSORCOMMAND_H
