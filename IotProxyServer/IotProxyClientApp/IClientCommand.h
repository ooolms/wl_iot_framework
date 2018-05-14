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
#include "ARpcBase/ARpcOutsideDevice.h"

class IClientCommand
	:public QObject
{
	Q_OBJECT
public:
	explicit IClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	static IClientCommand* mkCommand(CmdArgParser &p,ARpcOutsideDevice *d);
	static bool needToBeSilent(CmdArgParser &p);

public:
	virtual bool evalCommand()=0;

protected:
	void setExitErrorCode(int code);
	void processMessage(const ARpcMessage &m);
	virtual bool onOk(const QByteArrayList &args);
	virtual void onErr(const QByteArrayList &args);
	virtual bool onCmdData(const QByteArrayList &args);
	static QByteArrayList stringListToByteArrayList(const QStringList &list);

protected:
	const CmdArgParser &parser;
	ARpcOutsideDevice *dev;

protected:
	static const QByteArray addStorageCommand;
	static const QByteArray addStorageManualCommand;
	static const QByteArray bindSensorCommand;
	static const QByteArray devicesConfigCommand;
	static const QByteArray execCommandCommand;
	static const QByteArray getSamplesCommand;
	static const QByteArray getSamplesCountCommand;
	static const QByteArray identifyTcpCommand;
	static const QByteArray identifyTtyCommand;
	static const QByteArray jsProgramCommand;
	static const QByteArray listCommandsCommand;
	static const QByteArray listIdentifiedCommand;
	static const QByteArray listSensorsCommand;
	static const QByteArray listStoragesCommand;
	static const QByteArray listTtyCommand;
	static const QByteArray registerVirtualDeviceCommand;
	static const QByteArray removeStorageCommand;
	static const QByteArray sessionGetAttrCommand;
	static const QByteArray sessionGetWriteIdCommand;
	static const QByteArray sessionListAttrsCommand;
	static const QByteArray sessionListCommand;
	static const QByteArray sessionSetAttrCommand;
	static const QByteArray sessionStartCommand;
	static const QByteArray sessionStopCommand;
	static const QByteArray subscribeCommand;
	static const QByteArray unsubscribeCommand;
	static const QByteArray vdevMeasCommand;

private:
	int exitErrorCode;
};

#endif // ICLIENTCOMMAND_H

