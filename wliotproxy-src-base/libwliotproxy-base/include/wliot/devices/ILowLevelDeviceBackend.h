#ifndef ILOWLEVELDEVICEBACKEND_H
#define ILOWLEVELDEVICEBACKEND_H

#include <QObject>

namespace WLIOT
{
	class RealDevice;

	/**
	 * @brief Backend низкого уровня для устройства, работающий с потоком байт.
	 * Может так быть реализован для блочных устройств без особых потерь, так как
	 * данные в него передаются не побайтово, а как правило целыми сообщениями.
	 */
	class ILowLevelDeviceBackend
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ILowLevelDeviceBackend(const QString &hwAddress,QObject *parent=nullptr);
		virtual ~ILowLevelDeviceBackend(){}
		/**
		 * @brief Возвращает "аппаратный" адрес устройства (имя tty порта, ip адрес и т.п.)
		 * @return
		 */
		QString hwAddress()const;
		/**
		 * @brief Подождать, пока не установится соединение с устройством
		 * @param msecs время ожидания в миллисекундах
		 * @return
		 */
		virtual bool waitForConnected(int msecs=30000)=0;
		/**
		 * @brief Отправить данные устройству
		 * @param data данные
		 * @return
		 */
		virtual bool writeData(const QByteArray &data)=0;
		/**
		 * @brief Если канал связи с устройством буферизован, записать текущее содержимое буфера на устройство.
		 * @return
		 */
		virtual bool flush()=0;
		/**
		 * @brief isConnected
		 * @return
		 */
		virtual bool isConnected()const=0;
		/**
		 * @brief forceDisconnect
		 */
		virtual void forceDisconnect()=0;
		/**
		 * @brief backendType
		 * @return
		 */
		virtual QByteArray backendType()const=0;

	signals:
		/**
		 * @brief Сигнал о подключении устройства
		 */
		void connected();
		/**
		 * @brief Сигнал об отключении устройства
		 */
		void disconnected();
		/**
		 * @brief Сигнал о появлении новых данных от устройства
		 * @param data данные
		 */
		void newData(const QByteArray &data);

	private:
		QString mHwAddress;
	};
}

#endif // ILOWLEVELDEVICEBACKEND_H
