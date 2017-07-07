#ifndef ICOMMAND_H
#define ICOMMAND_H

#include "ARpcBase/ARpcOutsideDevice.h"

class ICommand
{
public:
	explicit ICommand(ARpcOutsideDevice *d);
	virtual ~ICommand(){}
	virtual bool processCommand(const ARpcMessage &m)=0;
	virtual QStringList acceptedCommands()=0;
	const QString& lastError();

protected:
	ARpcOutsideDevice *clientDev;
	QString lastErrorStr;
};

#endif // ICOMMAND_H
