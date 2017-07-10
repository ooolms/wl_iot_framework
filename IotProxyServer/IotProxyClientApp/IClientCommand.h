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
	static IClientCommand* mkCommand(CmdArgParser &p,ARpcOutsideDevice *d,QObject *parent=0);

public:
	virtual bool evalCommand()=0;

protected:
	const CmdArgParser &parser;
	ARpcOutsideDevice *dev;

protected:
	static const QString listTtyCommand;
	static const QString identifyTtyCommand;
};

#endif // ICLIENTCOMMAND_H
