#ifndef IDENTIFYTTYCOMMAND_H
#define IDENTIFYTTYCOMMAND_H

#include "../IClientCommand.h"

class IdentifyTtyCommand
	:public IClientCommand
{
public:
	IdentifyTtyCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent=0);

public:
	virtual bool evalCommand()override;
};

#endif // IDENTIFYTTYCOMMAND_H
