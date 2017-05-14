#ifndef LISTTTYCLIENTCOMMAND_H
#define LISTTTYCLIENTCOMMAND_H

#include "IClientCommand.h"

class ListTtyClientCommand
	:public IClientCommand
{
	Q_OBJECT
public:
	explicit ListTtyClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent=0);
	virtual bool evalCommand()override;

private slots:
	void onRawMessage(const ARpcMessage &m);
};

#endif // LISTTTYCLIENTCOMMAND_H
