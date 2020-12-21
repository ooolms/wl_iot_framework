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

#ifndef REALDEVICE_H
#define REALDEVICE_H

#include "wliot/devices/Message.h"
#include "wliot/devices/SensorDef.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/devices/DeviceState.h"
#include "wliot/devices/CommandCall.h"
#include "wliot/devices/IHighLevelDeviceBackend.h"
#include <QTimer>
#include <QUuid>
#include <QSharedPointer>

namespace WLIOT
{
	class HubDeviceBackend;

	/**
	 * @brief Устройство с идентификатором, именем, сенсорами, интерфейсом управления, состоянием и т.д.
	 * @details Данный класс реализует весь процесс взаимодействия с устройством по протоколу:
	 * процесс идентификации, запрос списка датчиков и интерфейса управления, запрос состояния,
	 * выполнение команд и т.д.
	 */
	class RealDevice
		:public QObject
	{
		Q_OBJECT
	public:
		/**
		 * @brief Результат процесса идентификации
		 */
		enum IdentifyResult
		{
			FAILED_ID_MISMATCH=-1,///< устройство уже идентифицировано ранее и id не совпадают
			FAILED,///< команда не вополнена
			OK,///< успешно
			OK_NULL_ID_OR_NAME,///< команда выполнена, но id нулевой или имя пустое
		};

	public:
		explicit RealDevice(QObject *parent=nullptr);
		explicit RealDevice(const QUuid &id,const QByteArray &name,QObject *parent=nullptr);
		virtual ~RealDevice();
		/**
		 * @brief установить backend
		 * @details Если текущий backend установлен и подключен, будет выдан сигнал disconnected().
		 * Затем произойдет замена backend-а. Если новый backend подключен, будет выыдан сигнал connected()
		 * @param b backend
		 */
		void setBackend(IHighLevelDeviceBackend *b);
		/**
		 * @brief "Забрать" backend устройства, используется в определенных случаях сервером
		 * @return
		 */
		IHighLevelDeviceBackend* takeBackend();
		IHighLevelDeviceBackend* backend();
		/**
		 * @brief Идентификация устройтва
		 * @details Производит запрос на идентификацию. Если устройство было уже идентифицировано ранее,
		 * проверяет, что id не изменился, однако могут измениться остальные параметры:
		 * имя, список датчиков, интерфейс управления и т.д.
		 * @return результат идентификации
		 */
		IdentifyResult identify();
		/**
		 * @brief возвращает true, если устройство подключено и идентифицировано
		 * @return
		 */
		bool isReady();
		QUuid id();
		QUuid typeId();
		QByteArray name();//human-readable
		void renameDevice(const QByteArray &newName,bool silent=true);
			//silent - name updated without emiting nameChanged signal
		bool getSensorsDescription(QList<SensorDef> &sensors);
		bool getControlsDescription(ControlsGroup &controls);
		bool getState(DeviceState &state);
		/**
		 * @brief Отправка сообщения устройству
		 * @param m сообщение
		 * @return
		 */
		bool writeMsgToDevice(const Message &m);
		bool isConnected()const;
		QSharedPointer<CommandCall> execCommand(CommandCall *call);
		QSharedPointer<CommandCall> execCommand(const QByteArray &cmd);
		QSharedPointer<CommandCall> execCommand(const QByteArray &cmd,const QByteArrayList &args);

	public://for hub devices
		bool isHubDevice();
		QList<QUuid> childDevices();
		HubDeviceBackend* childDevice(const QUuid &id);
		bool identifyHub();

	signals:
		/**
		 * @brief сигнал об отключении устройства
		 */
		void disconnected();
		/**
		 * @brief сигнал о подключении устройства
		 */
		void connected();
		/**
		 * @brief сигнал об идентификации устройства
		 * @details Сигнал может выдаваться либо при первой идентификации, либо например при смене имени,
		 * списка датчиков и т.д. Выдаваться повторно с другим идентификатором от того же устройства он не может
		 */
		void identified();
		/**
		 * @brief сигнал о приходе нового сообщения от устройства
		 */
		void newMessageFromDevice(const WLIOT::Message &m);
		/**
		 * @brief сигнал о том, что устройтво было сброшено (известное ранее состояние устройства стало невалидным)
		 */
		void deviceWasReset();
		/**
		 * @brief сигнал об изменении состояния устройства
		 */
		void stateChanged(const QByteArrayList &args);
		/**
		 * @brief сигнал об изменении состояния устройства
		 */
		void nameChanged(const QByteArray &newName);
		/**
		 * @brief сигнал о таймауте синхронизации, за ним последует сигнал об отключении устройства
		 */
		void syncFailed();

		void childDeviceIdentified(const QUuid &deviceId);
		void childDeviceLost(const QUuid &deviceId);

	protected slots://internal API
		void onConnected();
		void onDisconnected();
		void onDeviceReset();
		void onNewMessage(const WLIOT::Message &m);

	private slots:
		void onSyncTimer();
		void onCommandDone();
		void onBackendDestroyed();
		void onChildDeviceDisconnectedForced();

	private:
		void onHubMsg(const Message &m);
		void onHubDeviceIdentified(const QUuid &id,const QByteArray &name,const QUuid &typeId);
		void stopCommands(CommandCall::Error err);

	protected://для потомков
		QUuid devId;
		QByteArray devName;
		QUuid devTypeId;
		bool hubDevice;
		QTimer syncTimer;
		int mSyncCounter;

	private:
		IHighLevelDeviceBackend *mBackend;
		QMap<QUuid,HubDeviceBackend*> hubDevicesMap;
		QMap<QByteArray,QSharedPointer<CommandCall>> execCommands;
		CommandCall *identifyCall;
		QList<SensorDef> mSensors;
		ControlsGroup mControls;
		DeviceState mState;
		bool mControlsLoaded;
		bool mSensorsLoaded;
		bool mStateLoaded;
		bool mConnected;
		qint64 callId;
	};
}

#endif // REALDEVICE_H
