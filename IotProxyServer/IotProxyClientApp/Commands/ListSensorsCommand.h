#ifndef LISTSENSORSCOMMAND_H
#define LISTSENSORSCOMMAND_H

#include "../IClientCommand.h"

class ListSensorsCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit ListSensorsCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;

private slots:
	void onRawMessage(const ARpcMessage &m);
};

#endif // LISTSENSORSCOMMAND_H
