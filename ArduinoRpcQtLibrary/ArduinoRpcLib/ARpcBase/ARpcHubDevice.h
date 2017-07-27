#ifndef ARPCHUBDEVICE_H
#define ARPCHUBDEVICE_H

#include "ARpcDevice.h"

class ARpcHubDevice
	:public ARpcDevice
{
	Q_OBJECT
public:
	explicit ARpcHubDevice(const QUuid id,const QString &name,ARpcDevice *parent=0);
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

private:
	ARpcDevice *parentDevice;
};

#endif // ARPCHUBDEVICE_H
