#ifndef ILOWLEVELDEVICEBACKEND_H
#define ILOWLEVELDEVICEBACKEND_H

#include <QObject>

namespace WLIOT
{
	class RealDevice;

	class ILowLevelDeviceBackend
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ILowLevelDeviceBackend(const QString &hwAddress,QObject *parent=nullptr);
		virtual ~ILowLevelDeviceBackend(){}
		QString hwAddress()const;
		virtual bool waitForConnected(int msecs=30000)=0;
		virtual bool writeData(const QByteArray &data)=0;
		virtual bool flush()=0;
		virtual bool isConnected()const=0;
		virtual void forceDisconnect()=0;
		virtual QByteArray backendType()const=0;

	signals:
		void connected();
		void disconnected();
		void newData(const QByteArray &data);

	private:
		QString mHwAddress;
	};
}

#endif // ILOWLEVELDEVICEBACKEND_H
