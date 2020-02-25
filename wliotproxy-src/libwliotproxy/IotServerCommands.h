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

#ifndef IOTSERVERCOMMANDS_H
#define IOTSERVERCOMMANDS_H

#include <QObject>
#include "IotServerDevicesCommands.h"
#include "IotServerStoragesCommands.h"
#include "IotServerConfigurationCommands.h"
#include "IotServerAccessPolicyCommands.h"
#include "IotServerJSScriptsCommands.h"

class IotServerCommands
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerCommands(IotServerConnection *conn);
	IotServerConfigurationCommands* config();
	IotServerDevicesCommands* devices();
	IotServerStoragesCommands* storages();
	IotServerAccessPolicyCommands* accessPolicy();
	IotServerJSScriptsCommands* jsScriptsCommands();

private:
	IotServerConfigurationCommands *mConfig;
	IotServerDevicesCommands *mDevices;
	IotServerStoragesCommands *mStorages;
	IotServerAccessPolicyCommands *mApm;
	IotServerJSScriptsCommands *mJSScripts;
};

#endif // IOTSERVERCOMMANDS_H
