#ifndef EXECCOMMANDCOMMAND_H
#define EXECCOMMANDCOMMAND_H

#include "../IClientCommand.h"

class ExecCommandCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit ExecCommandCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;

private slots:
	void onRawMessage(const ARpcMessage &m);
};

#endif // EXECCOMMANDCOMMAND_H
