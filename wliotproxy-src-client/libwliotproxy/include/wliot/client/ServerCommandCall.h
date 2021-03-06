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

#ifndef SERVERCOMMANDCALL_H
#define SERVERCOMMANDCALL_H

#include <QObject>
#include <QTimer>
#include <QByteArrayList>
#include <QEventLoop>
#include "wliot/client/ServerDataTypes.h"

namespace WLIOTClient
{
	class ServerConnection;

	/**
	 * @brief Класс, упарвляющий выполнением команды серверу
	 */
	class ServerCommandCall
		:public QObject
	{
		Q_OBJECT
	public:
		/**
		 * @brief Конструктор
		 * @param conn соединение
		 * @param onCmData callback-функция, которая будет вызываться при приходе от сервера сообщений cmdata
		 * в процессе выполнения команды, может быть 0, если обработка этих сообщений в команде не предусмотрена
		 * @param cmd команда
		 * @param args аргументы команды
		 * @param parent
		 */
		explicit ServerCommandCall(ServerConnection *conn,CmDataCallback onCmData,const QByteArray &cmd,
			const QByteArrayList &args,QObject *parent=nullptr);
		/**
		 * @brief запустить команду и дождаться ответа от сервера
		 */
		void call();
		/**
		 * @brief возвращает true, если команда успешно выполнена
		 */
		bool ok();
		/**
		 * @brief возвращенный сервером результат выполнения команды
		 */
		const QByteArrayList& returnValue();

	private slots:
		void onMessage(const WLIOT::Message &m);
		void onDisconnected();
		void onTimeout();
		void onConnected();

	private:
		Q_INVOKABLE void setDone();

	protected:
		QTimer tmr;
		ServerConnection *srvConn;
		QByteArray mCmd;
		QByteArrayList mArgs,retVal;
		CmDataCallback mOnCmData;
		QByteArray callId;
		bool done;
		bool mOk;
	};
}

#endif // SERVERCOMMANDCALL_H
