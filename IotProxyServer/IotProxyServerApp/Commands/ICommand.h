#ifndef ICOMMAND_H
#define ICOMMAND_H

#include "ARpcBase/ARpcOutsideDevice.h"

class ICommand
{
public:
	explicit ICommand(ARpcOutsideDevice *d);
	virtual ~ICommand(){}
	virtual bool processCommand(const ARpcMessage &m)=0;

protected:
	ARpcOutsideDevice *clientDev;
};

#endif // ICOMMAND_H
