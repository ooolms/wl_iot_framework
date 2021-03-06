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

#ifndef VIRTUALDEVICECALLBACK_H
#define VIRTUALDEVICECALLBACK_H

#include <QObject>
#include "wliot/devices/DeviceState.h"

namespace WLIOTClient
{
	class VirtualDeviceClient;

	/**
	* @brief Callback-класс для регистрируемых виртуальных устройств
	*/
	class VirtualDeviceCallback
	{
	public:
		explicit VirtualDeviceCallback();
		virtual ~VirtualDeviceCallback(){}

	protected:
		/**
		 * @brief Вызывается библиотекой, когда от сервера приходит команда виртуальному устройству
		 * @param cmd команда
		 * @param args аргументы
		 * @param[out] retVal возвращаемое значение
		 * @return нужно вернуть true, если команда обработана успешно
		 */
		virtual bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)=0;
		/**
		 * @brief Вызывается библиотекой для инициализации состояния вируального устройства
		 */
		virtual void prepareState(WLIOT::DeviceState &);

	protected:
		friend class DevicesList;
		friend class VirtualDeviceClient;
		VirtualDeviceClient *devClient;
	};
}

#endif // VIRTUALDEVICECALLBACK_H
