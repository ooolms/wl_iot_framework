#ifndef ICLIENTCOMMAND_H
#define ICLIENTCOMMAND_H

#include <QObject>
#include "CmdArgParser.h"
#include "ARpcBase/ARpcOutsideDevice.h"

class IClientCommand
	:public QObject
{
	Q_OBJECT
public:
	explicit IClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent=0);
	virtual bool evalCommand()=0;
	static IClientCommand* mkCommand(const CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent=0);

protected:
	const CmdArgParser &parser;
	ARpcOutsideDevice *dev;
};

#endif // ICLIENTCOMMAND_H
