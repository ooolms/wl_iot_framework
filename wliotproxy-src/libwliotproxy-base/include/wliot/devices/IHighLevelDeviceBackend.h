#ifndef IHIGHLEVELDEVICEBACKEND_H
#define IHIGHLEVELDEVICEBACKEND_H

#include <QObject>
#include "wliot/devices/Message.h"

namespace WLIOT
{
	class IHighLevelDeviceBackend
		:public QObject
	{
		Q_OBJECT
	public:
		explicit IHighLevelDeviceBackend(QObject *parent=nullptr);
		virtual ~IHighLevelDeviceBackend(){}
		virtual bool writeMessageToDevice(const Message &m)=0;
		virtual bool isConnected()const=0;
		virtual void forceDisconnect()=0;
		virtual QByteArray type()const=0;
		virtual QByteArray portOrAddress()const=0;

	signals:
		void newMessageFromDevice(const WLIOT::Message &m);
		void connected();
		void disconnected();
		void deviceReset();
	};
}

#endif // IHIGHLEVELDEVICEBACKEND_H
