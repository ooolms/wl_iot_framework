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

#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H

#include <QObject>
#include "wliot/client/ServerConnection.h"
#include "wliot/client/AllServerCommands.h"
#include "wliot/client/DevicesList.h"
#include "wliot/client/StoragesDatabase.h"

namespace WLIOTClient
{
	/**
	 * @brief Класс для соединения с сервером
	 * @details Взаимодействие с сервером разделено на несколько уровней:
	 * 1. Уровень соединения (ServerConnection). Обеспечивает поддержку синхронизации,
	 * позволяет отправлять любые сообщения серверу
	 * 2. Уровень команд (AllServerCommands). Здесь представлены все
	 * поддерживаемые команды серверу, сгруппированные по назначению.
	 * 3. Верхний уровень (DevicesList, StoragesDatabase, VDILPrograms и т.д.).
	 */
	class ServerInstance
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ServerInstance(QObject *parent=nullptr);
		ServerConnection* connection();
		AllServerCommands* commands();
		DevicesList* devices();
		StoragesDatabase* storages();
		QByteArray findDevName(const QUuid &devId);

	private:
		ServerConnection *conn;
		AllServerCommands *mCommands;
		DevicesList *mDevices;
		StoragesDatabase *mStorages;
	};
}

#endif // SERVERINSTANCE_H
