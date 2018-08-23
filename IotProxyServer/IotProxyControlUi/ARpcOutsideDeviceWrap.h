#ifndef ARPCOUTSIDEDEVICEWRAP_H
#define ARPCOUTSIDEDEVICEWRAP_H

#include "ARpcBase/ARpcOutsideDevice.h"
#include "ARpcBase/ARpcRealDevice.h"
#include <QUuid>

class ARpcOutsideDeviceWrap
	:public ARpcRealDevice
{
	Q_OBJECT
public:
	explicit ARpcOutsideDeviceWrap(const QByteArray &devIdOrName,ARpcOutsideDevice *srvSocketDev,QObject *parent=0);

public:
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

private slots:
	void onRawMessage(const ARpcMessage &m);

private:
	QByteArray deviceIdOrName;
	QMap<QByteArray,QByteArray> callIdsMap;//callIdToServer -> callIdFromUi
	ARpcOutsideDevice *srvSock;
};

#endif // ARPCOUTSIDEDEVICEWRAP_H
