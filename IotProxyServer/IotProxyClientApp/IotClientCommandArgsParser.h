/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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
