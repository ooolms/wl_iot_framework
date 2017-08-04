#ifndef ARPCOUTSIDEDEVICEWRAP_H
#define ARPCOUTSIDEDEVICEWRAP_H

#include "ARpcBase/ARpcOutsideDevice.h"
#include <QUuid>

class ARpcOutsideDeviceWrap
	:public ARpcDevice
{
	Q_OBJECT
public:
	explicit ARpcOutsideDeviceWrap(const QString &devIdOrName,ARpcOutsideDevice *srvSocketDev,QObject *parent=0);

public:
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

private:
	QString deviceIdOrName;
	ARpcOutsideDevice *srvSock;
};

#endif // ARPCOUTSIDEDEVICEWRAP_H
