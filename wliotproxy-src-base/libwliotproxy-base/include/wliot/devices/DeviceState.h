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

#ifndef DEVICESTATE_H
#define DEVICESTATE_H

#include <QMap>
#include <QByteArrayList>
#include "wliot/devices/ControlsDefinition.h"

namespace WLIOT
{
	/**
	 * @brief Состояние устройства
	 */
	class DeviceState
	{
	public:
		/**
		 * @brief Преобразование состояния в список аргументов для команды #state
		 */
		QByteArrayList dumpToMsgArgs()const;
		/**
		 * @brief Преобразование состояния из списка аргументов, возвращаемых устройством
		 * в ответ на команду #state
		 * @param args аргументы
		 * @return
		 */
		bool parseMsgArgs(const QByteArrayList &args);
		/**
		 * @brief Сгенерировать состояние из описания элементов управнения
		 * @param commands список команд
		 * @return
		 */
		static DeviceState makeFromCommands(const QList<ControlsCommand> &commands);

	public:
		/**
		 * @brief Состояние, соответствующее элементам интерфейса управления, отображаемым для
		 * параметров команд. Например, есть команда light для включения света с одним параметром, которому
		 * соответствует элемент управления чекбокс. Этому параметру будет соответствовать элемент в QMap
		 * { "light" => {0 => "VALUE"}}, где VALUE принимает значения 0 или 1. При отображении интерфейса
		 * управления чекбокс будет сразу находиться в нужном состоянии в зависимости от того, включен свет или нет.
		 */
		QMap<QByteArray,QMap<quint32,QByteArray>> commandParams;
		/**
		 * @brief Дополнительные параметры состояния устройства, не связанные с инртерфейсом управления
		 */
		QMap<QByteArray,QByteArray> additionalAttributes;
	};
}

#endif // DEVICESTATE_H
