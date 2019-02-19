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
#include "../wliotproxyd/CmdArgParser.h"
#include "ARpcBase/ARpcOutsideDevice.h"

class IClientCommand
	:public QObject
{
	Q_OBJECT
public:
	explicit IClientCommand(const CmdArgParser &p,ARpcOutsideDevice *d);
	static IClientCommand* mkCommand(CmdArgParser &p,ARpcOutsideDevice *d);
	bool forCompletion();

public:
	virtual bool evalCommand()=0;

protected:
	void setExitErrorCode(int code);
	void processMessage(const ARpcMessage &m);
	virtual bool onOk(const QByteArrayList &args);
	virtual void onErr(const QByteArrayList &args);
	virtual bool onCmdData(const QByteArrayList &args);
	static QByteArrayList stringListToByteArrayList(const QStringList &list);
	bool writeCommandToServer(const QByteArray &cmd,const QByteArrayList &args=QByteArrayList());

protected:
	const CmdArgParser &parser;

protected:
	static const QByteArray addStorageCommand;
	static const QByteArray apmCommand;
	static const QByteArray availableDataExportServicesCommand;
	static const QByteArray changeDeviceOwnerCommand;
	static const QByteArray dataExportCommand;
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
	static const QByteArray sessionCommand;
	static const QByteArray storageGetAttrCommand;
	static const QByteArray storageSetAttrCommand;
	static const QByteArray subscribeCommand;
	static const QByteArray unsubscribeCommand;
	static const QByteArray vdevMeasCommand;

private:
	static bool mForCompletion;
	int exitErrorCode;
	ARpcOutsideDevice *dev;
	QByteArray callId;
};

#endif // ICLIENTCOMMAND_H

