#ifndef FAKEDEVICE_H
#define FAKEDEVICE_H

#include "ARpcBase/ARpcDevice.h"

class FakeDevice
	:public ARpcDevice
{
	Q_OBJECT
public:
	explicit FakeDevice(QObject *parent=0);

public:
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

signals:
	void logMsg(const QString &s);

private:
	QUuid devId;
};

#endif // FAKEDEVICE_H
