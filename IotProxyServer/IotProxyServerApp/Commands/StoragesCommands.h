#ifndef STORAGESCOMMANDS_H
#define STORAGESCOMMANDS_H

#include "ICommand.h"

class StoragesCommands
	:public ICommand
{
public:
	explicit StoragesCommands(ARpcOutsideDevice *d);

public:
	virtual bool processCommand(const ARpcMessage &m)override;
	virtual QStringList acceptedCommands()override;

private:
	bool listStorages(const ARpcMessage &m);
	bool addSensor(const ARpcMessage &m);
	bool removeSensor(const ARpcMessage &m);
};

#endif // STORAGESCOMMANDS_H
