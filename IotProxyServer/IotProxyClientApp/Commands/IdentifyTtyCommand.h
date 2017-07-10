#ifndef IDENTIFYTTYCOMMAND_H
#define IDENTIFYTTYCOMMAND_H

#include "../IClientCommand.h"

class IdentifyTtyCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit IdentifyTtyCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;

private slots:
	void onRawMessage(const ARpcMessage &m);
};

#endif // IDENTIFYTTYCOMMAND_H
