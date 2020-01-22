#ifndef HUBDEVICEBACKEND_H
#define HUBDEVICEBACKEND_H

#include "wliot/devices/IHighLevelDeviceBackend.h"

class RealDevice;
class HubDevice;

class HubDeviceBackend
	:public IHighLevelDeviceBackend
{
	Q_OBJECT
public:
	explicit HubDeviceBackend(RealDevice *pDev,HubDevice *hDev);
	bool writeMessageToDevice(const Message &m);
	bool isConnected()const;
	void forceDisconnect();
	void setSelfConnected(bool c);

private slots:
	void onParentConnected();
	void onParentDisconnected();

private:
	RealDevice *parentDevice;
	HubDevice *hubDev;
	bool mSelfConnected;
};

#endif // HUBDEVICEBACKEND_H
