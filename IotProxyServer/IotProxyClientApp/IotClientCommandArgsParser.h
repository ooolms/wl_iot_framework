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
	explicit IotClientCommandArgsParser(int argc,char **argv,QObject *parent=0);
	bool getCommandStatus();

private slots:
	void onRawMessage(const ARpcMessage &m);

private:
	bool status;
	CmdArgParser parser;
	QLocalSocket *sock;
	ARpcOutsideDevice *dev;
	IClientCommand *cmd;
};

#endif // IOTCLIENTCOMMANDARGSPARSER_H
