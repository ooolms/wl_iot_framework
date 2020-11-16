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

#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <QObject>
#include <QLocalSocket>
#include <QSslSocket>
#include <QByteArrayList>
#include <QNetworkProxy>
#include <QTimer>
#include <QThread>
#include "wliot/WLIOTServerProtocolDefs.h"
#include "wliot/client/ServerDataTypes.h"
#include "wliot/devices/StreamParser.h"
#include "wliot/devices/SensorValue.h"
#include "wliot/storages/StorageId.h"

namespace WLIOTClient
{
	class ServerConnectionSocketWrap;

	/**
	* @brief Класс, обеспечивающий соединение с сервером
	*/
	class ServerConnection
		:public QObject
	{
		/**
		 * @brief Тип соединения
		 */
		enum Type
		{
			UnixSock,///< unix socket /tmp/wliotproxyd
			TcpSock,///< tcp соединение
			Stdio///< соединение через stdio/stdout для программ обработки данных, запускаемых самим сервером
		};

		Q_OBJECT
	public:
		explicit ServerConnection(QObject *parent=nullptr);
		virtual ~ServerConnection();
		/**
		 * @brief Установить таймаут для попытки повторного подключения в случае обрыва соединения
		 * @param msec таймаут в миллисекундах (0 - не пытаться повторно подключаться)
		 */
		void setAutoReconnect(int msec);//0 - no auto reconnect
		/**
		 * @brief Подготовить данные для аутентификации
		 * @param user логин
		 * @param pass пароль
		 */
		void prepareAuth(const QByteArray &user,const QByteArray &pass);
		/**
		 * @brief Аутентифицироваться на сервере, можно сменить пользователя таким образом уже в процессе работы
		 * @param user логин
		 * @param pass пароль
		 */
		bool authenticate(const QByteArray &user,const QByteArray &pass);
		/**
		 * @brief если передать true, отладочного вывода не будет
		 * @param n
		 */
		void setNoDebug(bool n);
		/**
		 * @brief Начать соединение по unix сокету
		 * @return
		 */
		bool startConnectLocal();
		/**
		 * @brief Начать соединение по tcp сокету
		 * @return
		 */
		bool startConnectNet(const QString &host,quint16 port=WLIOT::WLIOTServerProtocolDefs::controlSslPort);
		/**
		 * @brief Начать "соединение" через stdio/stdout
		 * @return
		 */
		bool startConnectStdio();
		/**
		 * @brief Возвращает true, если соединение установлено
		 * @return
		 */
		bool isConnected();
		/**
		 * @brief Возвращает true, если соединение установлено и пройдена аутентификация
		 * @return
		 */
		bool isReady();//connected and authenticated
		/**
		 * @brief Выполнить команду на сервере
		 * @param cmd команда
		 * @param args аргументы
		 * @param[out] retVal результат выполнения команды
		 * @param onCmData функция-обработчик для сообщений cmdata, может быть 0
		 * @return true, если команда выполнена успешно
		 */
		bool execCommand(const QByteArray &cmd,const QByteArrayList &args,
			QByteArrayList &retVal,CmDataCallback onCmData=0);
		/**
		 * @brief Выполнить команду на сервере
		 * @param cmd команда
		 * @param args аргументы
		 * @param onCmData функция-обработчик для сообщений cmdata, может быть 0
		 * @return true, если команда выполнена успешно
		 */
		bool execCommand(const QByteArray &cmd,const QByteArrayList &args,CmDataCallback onCmData=0);
		/**
		 * @brief Подождать, пока подключение к серверу не будет установлено (аутентификация сожет быть еще не пройдена)
		 * @param msec время в миллисекундах
		 * @return
		 */
		bool waitForConnected(int msec=30000);
		/**
		 * @brief Отключиться от сервера
		 */
		void disconnectFromServer();
		/**
		 * @brief Подпиcаться на уведомление о появлении новых данных в хранилище датчика
		 * @param devIdOrName идентификатор или имя устройства
		 * @param sensorName название датчика
		 * @return
		 */
		bool subscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName);
		/**
		 * @brief Отпиcаться от уведомлений о появлении новых данных в хранилище датчика
		 * @param devIdOrName идентификатор или имя устройства
		 * @param sensorName название датчика
		 * @return
		 */
		bool unsubscribeStorage(const QByteArray &devIdOrName,const QByteArray &sensorName);
		/**
		 * @brief Идентифицировать сервер
		 * @param[out] id идентификатор сервера
		 * @param[out] name название сервера
		 * @return
		 */
		bool identifyServer(QUuid &id,QByteArray &name);
		/**
		 * @brief отправить сообщение серверу
		 * @details Как правило, нет необходимости использовать напрямую. Данная функция используется более
		 * высокоуровневым API
		 * @param m сообщение
		 * @return
		 */
		bool writeMsg(const WLIOT::Message &m);
		/**
		 * @brief Установить прокси для tcp соединения, работать будет только Socks5 прокси
		 * @param p
		 */
		void setProxy(const QNetworkProxy &p);
		bool writeVDevMsg(const QUuid &id,const WLIOT::Message &m);
		/**
		 * @brief идентификатор текущего пользователя
		 * @return
		 */
		ApmIdType userId();//-1 if not authenticated or unknown user

	signals:
		/**
		 * @brief сигнал о подключении к серверу
		 */
		void connected();
		/**
		 * @brief сигнал об отключении от сервера
		 */
		void disconnected();
		/**
		 * @brief сигнал об ошибке соединения с сервером
		 */
		void connectionError();
		/**
		 * @brief Сигнал о том, что в хранилище на сервере появилось новое значение датчика
		 * @param id идентификатор хранилища
		 * @param valueArgs значение как в сообщении meas от устройства
		 */
		void newSensorValue(const WLIOT::StorageId &id,const QByteArrayList &valueArgs);
		/**
		 * @brief сигнал о подключении устройства на сервере
		 */
		void deviceIdentified(const QUuid &id,const QByteArray &name,const QUuid &typeId);
		/**
		 * @brief сигнал об отключении устройства на сервере
		 */
		void deviceLost(const QUuid &id);
		/**
		 * @brief сигнал об изменении состояния устройства
		 * @param id идентификатор устройства
		 * @param args агрументы аналогично сообщению statechanged от устройства
		 */
		void deviceStateChanged(const QUuid &id,const QByteArrayList &args);
		/**
		 * @brief сигнал о появлении нового хранилища на сервере
		 */
		void storageCreated(const WLIOTClient::StorageDescr &s);
		/**
		 * @brief сигнал об удалении хранилища с сервера
		 */
		void storageRemoved(const WLIOT::StorageId &id);
		void funcCallReplyMsg(const WLIOT::Message &m);
		void vdevMsg(const QUuid &id,const WLIOT::Message &m);
		void connectedForInternalUse();
		/**
		 * @brief сигнал об ошибке аутентификации
		 */
		void authFailed();

	private slots:
		void onLocalSocketConnected();
		void onNetSocketConnected();
		void onStdioConnected();
		void onDevDisconnected();
		void onRawMessage(const WLIOT::Message &m);
		void onSyncTimer();
		void onNewData(QByteArray data);
		void onConnectionError();
		void onReconnectTimer();

	private:
		void stopSockThread();
		ApmIdType authInternal();
		ApmIdType checkUserInfo();

	private:
		friend class ServerCommandCall;
		friend class ServerConnectionSocketWrap;

		WLIOT::StreamParser parser;
		QThread *sockThread;
		ServerConnectionSocketWrap *sock;
		Type connType;
		bool connectionReady;
		quint64 callIdNum;
		QNetworkProxy proxy;
		ApmIdType mUid;
		QTimer syncTimer,reconnectTimer;
		QString mHost;
		quint16 mPort;
		QByteArray mUser,mPass;
		int mSyncCount;
		bool noDebug;
	};
}

#endif // SERVERCONNECTION_H
