#ifndef LISTSTORAGESCOMMAND_H
#define LISTSTORAGESCOMMAND_H

#include "../IClientCommand.h"

class ListStoragesCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit ListStoragesCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	virtual bool evalCommand()override;

private slots:
	void onRawMessage(const ARpcMessage &m);
};

#endif // LISTSTORAGESCOMMAND_H
