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

#ifndef IOTPROXYCOMMANDPROCESSOR_H
#define IOTPROXYCOMMANDPROCESSOR_H

#include "ARpcBase/ARpcOutsideDevice.h"
#include "Commands/ICommand.h"
#include "ARpcLocalStorage/ARpcLocalDatabase.h"
#include <QObject>
#include <QMap>

class IotProxyCommandProcessor
	:public QObject
{
	Q_OBJECT
public:
	explicit IotProxyCommandProcessor(ARpcOutsideDevice *d,bool needAuth,QObject *parent=0);
	virtual ~IotProxyCommandProcessor();

public slots:
	void onNewValueWritten(const ARpcISensorValue *value);

private slots:
	void onRawMessage(const ARpcMessage &m);
	void onDeviceIdentified(QUuid id,QByteArray name);
	void onDeviceLost(QUuid id);
	void onStorageCreated(const DeviceStorageId &id);
	void onStorageRemoved(const DeviceStorageId &id);

private:
	void addCommand(ICommand *c);

private:
	ARpcOutsideDevice *dev;
	QMap<QString,ICommand*> commandProcs;
	QList<ICommand*> commands;
	bool ifNeedAuth;
	bool authentificated;
};

#endif // IOTPROXYCOMMANDPROCESSOR_H
