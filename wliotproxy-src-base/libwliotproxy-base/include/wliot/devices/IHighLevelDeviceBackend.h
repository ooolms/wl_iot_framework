#ifndef IHIGHLEVELDEVICEBACKEND_H
#define IHIGHLEVELDEVICEBACKEND_H

#include <QObject>
#include "wliot/devices/Message.h"

namespace WLIOT
{
	class RealDevice;

	/**
	 * @brief Backend высокого уровня для устройства. Оперирует отдельными сообщениями @see Message.
	 */
	class IHighLevelDeviceBackend
		:public QObject
	{
		Q_OBJECT
	public:
		explicit IHighLevelDeviceBackend(QObject *parent=nullptr);
		virtual ~IHighLevelDeviceBackend();
		void setDevice(RealDevice *device);
		RealDevice* device();
		/**
		 * @brief Передать сообщение устройству
		 * @param m сообщение
		 * @return true - сообщение успешно отправлено
		 */
		virtual bool writeMessageToDevice(const Message &m)=0;
		/**
		 * @brief Возвращает true, если устройство подключено
		 * @return
		 */
		virtual bool isConnected()const=0;
		/**
		 * @brief Принудительно отключиться от устройства
		 */
		virtual void forceDisconnect()=0;
		/**
		 * @brief Возвращает "тип" backend-а
		 * @return
		 */
		virtual QByteArray backendType()const=0;

		/**
		 * @brief Возвращает "аппаратный" адрес устройства (имя tty порта, ip адрес и т.п.)
		 * @return
		 */
		virtual QString hwAddress()const=0;

	signals:
		/**
		 * @brief Сигнал о появлении сообщения от устройства
		 * @param m сообщение
		 */
		void newMessageFromDevice(const WLIOT::Message &m);
		/**
		 * @brief Сигнал о подключении устройства
		 */
		void connected();
		/**
		 * @brief Сигнал об отключении устройства
		 */
		void disconnected();
		/**
		 * @brief Сигнал о том, что устройство было "сброшено" (перезагружено)
		 */
		void deviceReset();

		/**
		 * @brief destroyedBeforeQObject
		 * !!! обязательно вызывать этот сигнал из наследников
		 */
		void destroyedBeforeQObject();

	private:
		RealDevice *mDev;
	};
}

#endif // IHIGHLEVELDEVICEBACKEND_H
