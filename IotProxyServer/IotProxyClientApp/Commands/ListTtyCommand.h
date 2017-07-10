#ifndef LISTTTYCOMMAND_H
#define LISTTTYCOMMAND_H

#include "../IClientCommand.h"

class ListTtyCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit ListTtyCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;

private slots:
	void onRawMessage(const ARpcMessage &m);
};

#endif // LISTTTYCOMMAND_H
