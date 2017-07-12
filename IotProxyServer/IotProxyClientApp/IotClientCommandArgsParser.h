#ifndef IOTCLIENTCOMMANDARGSPARSER_H
#define IOTCLIENTCOMMANDARGSPARSER_H

#include "ARpcBase/ARpcOutsideDevice.h"
#include "CmdArgParser.h"
#include "IClientCommand.h"
#include <QObject>
#include <QLocalSocket>

class IotClientCommandArgsParser
	:public QObject
{
	Q_OBJECT
public:
	enum CommandStatus
	{
		DONE,
		IN_WORK,
		ERROR
	};

public:
	explicit IotClientCommandArgsParser(int argc,char **argv,QObject *parent=0);
	virtual ~IotClientCommandArgsParser();
	CommandStatus getCommandStatus();

private slots:
	void onRawMessage(const ARpcMessage &m);

private:
	CommandStatus status;
	CmdArgParser parser;
	QLocalSocket *sock;
	ARpcOutsideDevice *dev;
	IClientCommand *cmd;
};

#endif // IOTCLIENTCOMMANDARGSPARSER_H
