#ifndef HUBDEVICEBACKEND_H
#define HUBDEVICEBACKEND_H

#include "wliot/devices/IHighLevelDeviceBackend.h"

namespace WLIOT
{
	class RealDevice;
	class HubDevice;

	class HubDeviceBackend
		:public IHighLevelDeviceBackend
	{
		Q_OBJECT
	public:
		explicit HubDeviceBackend(RealDevice *pDev,HubDevice *hDev);
		bool writeMessageToDevice(const Message &m)override;
		bool isConnected()const override;
		void forceDisconnect()override;
		void setSelfConnected(bool c);
		virtual QByteArray type()const override;
		virtual QByteArray portOrAddress()const override;

	private slots:
		void onParentConnected();
		void onParentDisconnected();

	public:
		static const QByteArray devType;

	private:
		RealDevice *parentDevice;
		HubDevice *hubDev;
		bool mSelfConnected;
	};
}

#endif // HUBDEVICEBACKEND_H
