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

#ifndef VIRTUALDEVICECLIENT_H
#define VIRTUALDEVICECLIENT_H

#include "wliot/devices/DeviceState.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/devices/SensorDef.h"
#include "wliot/client/ServerConnection.h"

namespace WLIOTClient
{
	class VirtualDeviceCallback;

	//TODO hub support
	//TODO сделать get методы для полей
	class VirtualDeviceClient
		:public QObject
	{
		Q_OBJECT
	public:
		/**
		 * @brief Передать на сервер значение датчика виртуального устройства
		 * @param sensor название датчика
		 * @param args значение в аргументах как в сообщении meas от устройства
		 */
		void sendVDevMeasurement(const QByteArray &sensor,const QByteArrayList &args);
		/**
		 * @brief Передать на сервер значение датчика виртуального устройства
		 * @param sensor название датчика
		 * @param val значение с датчика
		 */
		void sendVDevMeasurementB(const QByteArray &sensor,const WLIOT::SensorValue &val);
		/**
		 * @brief Передать на сервер значение датчика виртуального устройства
		 * @param sensor название датчика
		 * @param val значение как в сообщении measb от устройства
		 */
		void sendVDevMeasurementB(const QByteArray &sensor,const QByteArray &val);
		/**
		 * @brief Передать на сервер info сообщение от устройства
		 * @param args
		 */
		void writeInfo(const QByteArrayList &args);
		/**
		 * @brief Текущее состояние виртуального устройства
		 * @return
		 */
		const WLIOT::DeviceState& state()const;
		/**
		 * @brief Отправка серверу уведомления об изменении состояния устройства (параметр команды)
		 * @param cmd команда
		 * @param paramIndex индекс параметра (0-based)
		 * @param value новое значение
		 */
		void commandParamStateChanged(const QByteArray &cmd,quint32 paramIndex,const QByteArray &value);
		/**
		 * @brief Отправка серверу уведомления об изменении состояния устройства (дополнительный параметр)
		 * @param paramName название параметра
		 * @param value новое значение
		 */
		void additionalStateChanged(const QByteArray &paramName,const QByteArray &value);

	private:
		explicit VirtualDeviceClient(ServerConnection *conn,const QUuid &id,const QByteArray &name,
			const QUuid &typeId,const QList<WLIOT::SensorDef> &sensors,
			const WLIOT::ControlsGroup &controls,QObject *parent=nullptr);
		void onNewMessageFromServer(const WLIOT::Message &m);
		void writeOk(const QByteArray &callId,const QByteArrayList &args);
		void writeErr(const QByteArray &callId,const QByteArrayList &args);

	private:
		friend class DevicesList;
		friend class VirtualDeviceCallback;
		ServerConnection *srvConn;
		VirtualDeviceCallback *callback;
		QUuid devId;
		QUuid devTypeId;
		QByteArray devName;
		QList<WLIOT::SensorDef> mSensors;
		WLIOT::ControlsGroup mControls;
		WLIOT::DeviceState mState;
	};
}

#endif // VIRTUALDEVICECLIENT_H
