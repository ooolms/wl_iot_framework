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
	explicit IClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	static IClientCommand* mkCommand(CmdArgParser &p,ARpcOutsideDevice *d);

public:
	virtual bool evalCommand()=0;

protected:
	void setExitErrorCode(int code);
	void processMessage(const ARpcMessage &m);

protected:
	const CmdArgParser &parser;
	ARpcOutsideDevice *dev;

protected:
	static const QString listTtyCommand;
	static const QString identifyTtyCommand;
	static const QString listSensorsCommand;
	static const QString execCommandCommand;
	static const QString listStoragesCommand;
	static const QString addSensorCommand;
	static const QString removeSensorCommand;

private:
	int exitErrorCode;
};

#endif // ICLIENTCOMMAND_H
