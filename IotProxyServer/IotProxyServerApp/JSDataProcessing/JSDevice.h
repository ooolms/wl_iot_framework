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

#ifndef JSDEVICE_H
#define JSDEVICE_H

#include <QObject>
#include "ARpcBase/ARpcRealDevice.h"
#include <QScriptEngine>

class JSDevice
	:public QObject
{
	Q_OBJECT

public:
	explicit JSDevice(ARpcRealDevice *d,QScriptEngine *e,QObject *parent=nullptr);
	Q_INVOKABLE bool isIdentified();
	Q_INVOKABLE QString id();
	Q_INVOKABLE QString name();//human-readable
	Q_INVOKABLE QScriptValue getSensorsDescription();

	/**
	 * @brief sendCommand
	 * Отправить команду на устройство
	 * @param cmd строка с командой
	 * @param args список параметров команды (array), только строки, числа, bool(true преобразуется в 1, false - в 0),
	 *	даты (преобразуется с помощью toMSecsSinceEpoch)
	 * @return объект с ключами ok (true|false) и value (список строк, которые вернуло устройство)
	 */
	Q_INVOKABLE QScriptValue sendCommand(QScriptValue cmd,QScriptValue args);
	//TODO Q_INVOKABLE QScriptValue getControlsDescription();

signals:
	void identificationChanged();

protected:
	bool valToString(const QScriptValue &val,QString &str);
	QStringList arrayToStringList(QScriptValue arr);
	QScriptValue stringListToArray(const QStringList &list);

protected:
	ARpcRealDevice *dev;
	QScriptEngine *js;
};

#endif // JSDEVICE_H
