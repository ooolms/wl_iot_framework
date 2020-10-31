#ifndef IHIGHLEVELDEVICEBACKEND_H
#define IHIGHLEVELDEVICEBACKEND_H

#include <QObject>
#include "wliot/devices/Message.h"

namespace WLIOT
{
	class RealDevice;

	class IHighLevelDeviceBackend
		:public QObject
	{
		Q_OBJECT
	public:
		explicit IHighLevelDeviceBackend(QObject *parent=nullptr);
		virtual ~IHighLevelDeviceBackend();
		void setDevice(RealDevice *device);
		RealDevice* device();
		virtual bool writeMessageToDevice(const Message &m)=0;
		virtual bool isConnected()const=0;
		virtual void forceDisconnect()=0;
		virtual QByteArray backendType()const=0;

		/**
		 * @brief hwAddress
		 * @return hardware identification address: port name, tcp address or something else
		 */
		virtual QString hwAddress()const=0;

	signals:
		void newMessageFromDevice(const WLIOT::Message &m);
		void connected();
		void disconnected();
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
