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

#ifndef CONFIGURATIONCOMMANDS_H
#define CONFIGURATIONCOMMANDS_H

#include <QObject>
#include "ServerConnection.h"

namespace WLIOTClient
{
	/**
	 * @brief Команды настройки параметров работы сервера
	 */
	class ConfigurationCommands
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ConfigurationCommands(ServerConnection *conn);
		bool getTtyFilterByPortName(QByteArray &filter);
		bool setTtyFilterByPortName(const QByteArray &filter);
		bool getTtyFilterByPidVid(QByteArray &filter);
		bool setTtyFilterByPidVid(const QByteArray &filter);
		bool getTcpFilter(QByteArray &filter);
		bool setTcpFilter(const QByteArray &filter);
		bool setDetectTcpDevices(bool enable);

	private:
		ServerConnection *srvConn;
	};
}

#endif // CONFIGURATIONCOMMANDS_H
