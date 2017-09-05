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

public:
	virtual bool evalCommand()=0;

protected:
	void setExitErrorCode(int code);
	void processMessage(const ARpcMessage &m);
	virtual bool onOk(const QStringList &args);
	virtual void onErr(const QStringList &args);
	virtual bool onCmdData(const QStringList &args);

protected:
	const CmdArgParser &parser;
	ARpcOutsideDevice *dev;

protected:
	static const QString listTtyCommand;
	static const QString listIdentifiedCommand;
	static const QString identifyTtyCommand;
	static const QString listSensorsCommand;
	static const QString execCommandCommand;
	static const QString listStoragesCommand;
	static const QString addSensorCommand;
	static const QString removeSensorCommand;
	static const QString bindSensorCommand;
	static const QString devicesConfigCommand;

private:
	int exitErrorCode;
};

#endif // ICLIENTCOMMAND_H
