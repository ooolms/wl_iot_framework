#ifndef REMOVESENSORCOMMAND_H
#define REMOVESENSORCOMMAND_H

#include "../IClientCommand.h"

class RemoveSensorCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit RemoveSensorCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;
};

#endif // REMOVESENSORCOMMAND_H
