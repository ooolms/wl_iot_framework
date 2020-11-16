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

#ifndef COMMANDCALL_H
#define COMMANDCALL_H

#include "wliot/WLIOTProtocolDefs.h"
#include "wliot/devices/Message.h"
#include <QObject>
#include <QTimer>

namespace WLIOT
{
	class RealDevice;

	/**
	 * @brief Класс, отвечающий за отправку команды устройству, отслеживание ее выполнения и возврат результата.
	 */
	class CommandCall
		:public QObject
	{
		Q_OBJECT
	private:
		enum
		{
			WAIT,
			EXEC,
			DONE
		}state;
		enum Error
		{
			DEV_DISCONNECTED,
			DEV_DESTROYED,
			DEV_RESET
		};

	public:
		/**
		 * @brief Конструктор
		 * @details Команда задается в конструкторе, далее при необходимости задаются параметры и
		 * настройки методами ниже, далее вызывается exec(). Если нужно дождаться результата, вызывается wait(),
		 * либо ждать сигнал done()
		 * @param cmd команда, отправляемая устройству
		 */
		explicit CommandCall(const QByteArray &cmd,QObject *parent=0);
		/**
		 * @brief Возвращает результат выполнения команды
		 * @return результат выполнения команды
		 */
		const QByteArrayList& returnValue();
		/**
		 * @brief Установить аргументы команды
		 * @param args аргументы команды
		 * @return this
		 */
		CommandCall* setArgs(const QByteArrayList &args);
		/**
		 * @brief Использовать или нет заголовок call (see https://dev.alterozoom.com/doc/doku.php?id=iot:%D0%BF%D1%80%D0%BE%D1%82%D0%BE%D0%BA%D0%BE%D0%BB_%D0%B4%D0%BB%D1%8F_%D1%83%D1%81%D1%82%D1%80%D0%BE%D0%B9%D1%81%D1%82%D0%B2)
		 * @param args аргументы команды
		 * @return this
		 */
		CommandCall* setUseCallMsg(bool u);
		/**
		 * @brief Установить таймаут выполнения команды
		 * @details По-умолчанию таймаут не установлен, команда может прерваться, если устройство отключится.
		 * @param msec время в миллисекундах
		 * @return this
		 */
		CommandCall* setupTimer(int msec);
		/**
		 * @brief Перезапустить команду при приходе от устройства сигнала о сбросе (see https://dev.alterozoom.com/doc/doku.php?id=iot:%D0%BF%D1%80%D0%BE%D1%82%D0%BE%D0%BA%D0%BE%D0%BB_%D0%B4%D0%BB%D1%8F_%D1%83%D1%81%D1%82%D1%80%D0%BE%D0%B9%D1%81%D1%82%D0%B2)
		 * @param en если true, то команда будет отправлена заново при сбросе устройства
		 * @return this
		 */
		CommandCall* setRecallOnDevReset(bool en);
		/**
		 * @brief Возвращает true, если команда была выполнена успешно (устройство вернуло ok)
		 * @return
		 */
		bool ok();
		/**
		 * @brief Прервать выполнение команды
		 */
		void abort();
		/**
		 * @brief Дождаться окончания выполнения команды
		 * @return результат выполнения команды (аналогично ok())
		 */
		bool wait();
		/**
		 * @brief Возвращает true, если команда была отправлена, но результат еще не получен
		 * @return
		 */
		bool isWorking();
		/**
		 * @brief Возвращает идентификатор вызова, отправленный устройству в сообщении call
		 * @return
		 */
		QByteArray callId();

	signals:
		/**
		 * @brief Сигнал, выдающийся при завершении команды (успешном или нет)
		 */
		void done();

	private slots:
		void onError(Error err);
		void onTimeout();

	private:
		void run(RealDevice *d,const QByteArray &callIdStr);
		void onOkMsg(const QByteArrayList &args);
		void onErrorMsg(const QByteArray &msg);
		void onErrorMsg(const QByteArrayList &args);

	private:
		friend class RealDevice;
		QByteArrayList retVal;
		bool mOk;
		RealDevice *dev;
		QTimer timer;
		QByteArray mCommand;
		QByteArrayList mArgs;
		bool mUseCallMsg,mRecallOnDevReset;
		QByteArray mCallId;
	};
}

#endif // COMMANDCALL_H
