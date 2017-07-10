#ifndef ADDSENSORCOMMAND_H
#define ADDSENSORCOMMAND_H

#include "../IClientCommand.h"

class AddSensorCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit AddSensorCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;
};

#endif // ADDSENSORCOMMAND_H
