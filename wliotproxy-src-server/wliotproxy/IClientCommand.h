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

#ifndef ICLIENTCOMMAND_H
#define ICLIENTCOMMAND_H

#include <QObject>
#include "CmdArgParser.h"
#include "wliot/client/ServerConnection.h"

class IClientCommand
	:public QObject
{
	Q_OBJECT
public:
	explicit IClientCommand(const CmdArgParser &p,WLIOTClient::ServerConnection *c);
	static IClientCommand* mkCommand(CmdArgParser &p,WLIOTClient::ServerConnection *c);
	bool forCompletion();

public:
	virtual bool evalCommand()=0;

protected:
	void setExitErrorCode(int code);
	void processMessage(const WLIOT::Message &m);
	virtual bool onOk(const QByteArrayList &args);
	virtual void onErr(const QByteArrayList &args);
	virtual bool onCmdData(const QByteArrayList &args);
	static QByteArrayList stringListToByteArrayList(const QStringList &list);
	bool writeCommandToServer(const QByteArray &cmd,const QByteArrayList &args=QByteArrayList());

protected:
	const CmdArgParser &parser;
	static bool mForCompletion;

protected:
	static const QString addStorageCommand;
	static const QString alterozoomAuthCommand;
	static const QString alterozoomListCommand;
	static const QString apmCommand;
	static const QString availableDataExportServicesCommand;
	static const QString dataExportCommand;
	static const QString devicesConfigCommand;
	static const QString devNamesCommand;
	static const QString execCommandCommand;
	static const QString vdilProgramCommand;
	static const QString getSamplesCommand;
	static const QString getSamplesCountCommand;
	static const QString identifyTcpCommand;
	static const QString identifyTtyCommand;
	static const QString jsProgramCommand;
	static const QString listCommandsCommand;
	static const QString listIdentifiedCommand;
	static const QString listSensorsCommand;
	static const QString listStoragesCommand;
	static const QString listTtyCommand;
	static const QString removeStorageCommand;
	static const QString sessionCommand;
	static const QString setDevNameCommand;
	static const QString storageGetAttrCommand;
	static const QString storageSetAttrCommand;
	static const QString terminateCommand;
	static const QString vdevMeasCommand;

	static const QString subscribeCommand;
	static const QString unsubscribeCommand;

private:
	int exitErrorCode;
	WLIOTClient::ServerConnection *conn;
	QByteArray callId;
};

#endif // ICLIENTCOMMAND_H

