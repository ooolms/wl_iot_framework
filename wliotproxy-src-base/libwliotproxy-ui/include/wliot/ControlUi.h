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

#ifndef CONTROLUI_H
#define CONTROLUI_H

#include <QWidget>
#include "wliot/devices/RealDevice.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/devices/DeviceState.h"
#include "wliot/devices/SimpleMsgDispatch.h"

namespace WLIOTUi
{
	class ControlUiGroup;

	/**
	 * @brief Виджет, генерирующий интерфейс управления устройством из описания, полученного с устройства.
	 * @details При отображении в интерфейсе управления команды есть несколько нюансов:
	 * 1. Если у команды 1 параметр и команда может быть отправлена при изменении состояния элемента управления
	 * (например, переключение чекбокса), то кнопка отправки по-умолчанию не отображается (если не задан forceBtn).
	 * Во всех остальных случаях поведение аналогично веб-формам: пользователь сначала настраивает все парамеры команды,
	 * потом нажимает кнопку отправки.
	 * 2. Если для команды отображается более одного элемента Ui, ее элементы объединяются в группу.
	 * Если у команды задан заголовок, он отображается вверху группы.
	 */
	class ControlUi
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit ControlUi(WLIOT::RealDevice *dev,const WLIOT::ControlsGroup &controlsDef,QWidget *parent=nullptr);
		void updateState(const WLIOT::DeviceState &state);

	signals:
		void deviceIsNotReady();

	private slots:
		void onDeviceDestroyed();
		void onExecuteCommand(const QByteArray &command,const QByteArrayList &args);
		void onCommandStateChanged(const QByteArray &command,quint32 index,const QByteArray &value);

	private:
		WLIOT::RealDevice *device;
		ControlUiGroup *mainGroup;
		WLIOT::SimpleMsgDispatch *msgDsp;
	};
}

#endif // CONTROLUI_H
