#ifndef HUBDEVICEBACKEND_H
#define HUBDEVICEBACKEND_H

#include "wliot/devices/IHighLevelDeviceBackend.h"
#include <QUuid>

namespace WLIOT
{
	class RealDevice;

	class HubDeviceBackend
		:public IHighLevelDeviceBackend
	{
		Q_OBJECT
	public:
		explicit HubDeviceBackend(const QUuid &devId,const QByteArray &name,const QUuid &typeId,RealDevice *pDev);
		virtual ~HubDeviceBackend();
		bool writeMessageToDevice(const Message &m)override;
		bool isConnected()const override;
		void forceDisconnect()override;
		void setSelfConnected(bool c);
		virtual QByteArray backendType()const override;
		virtual QString hwAddress()const override;
		QUuid devId();
		QByteArray devName();
		QUuid devTypeId();

	signals:
		void forceDisconnectCalled();

	private slots:
		void onParentConnected();
		void onParentDisconnected();

	private:
		void messageFromDevice(const Message &m);

	public:
		static const QByteArray mBackendType;
		const QUuid mId;
		const QByteArray mName;
		const QUuid mTypeId;

	private:
		friend class RealDevice;
		RealDevice *parentDevice;
		bool mSelfConnected;
	};
}

#endif // HUBDEVICEBACKEND_H
