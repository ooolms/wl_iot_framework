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

#ifndef ALLSERVERCOMMANDS_H
#define ALLSERVERCOMMANDS_H

#include <QObject>
#include "wliot/client/DevicesCommands.h"
#include "wliot/client/StoragesCommands.h"
#include "wliot/client/ConfigurationCommands.h"
#include "wliot/client/AccessPolicyCommands.h"
#include "wliot/client/JSScriptsCommands.h"
#include "wliot/client/VDILProgramsCommands.h"

namespace WLIOTClient
{
	/**
	 * @brief Все команды для сервера
	 */
	class AllServerCommands
		:public QObject
	{
		Q_OBJECT
	public:
		explicit AllServerCommands(ServerConnection *conn);
		ConfigurationCommands* config();
		DevicesCommands* devices();
		StoragesCommands* storages();
		AccessPolicyCommands* accessPolicy();
		JSScriptsCommands* jsScriptsCommands();
		VDILProgramsCommands* vdilPrograms();

	private:
		ConfigurationCommands *mConfig;
		DevicesCommands *mDevices;
		StoragesCommands *mStorages;
		AccessPolicyCommands *mApm;
		JSScriptsCommands *mJSScripts;
		VDILProgramsCommands *mVDILPrograms;
	};
}

#endif // ALLSERVERCOMMANDS_H
