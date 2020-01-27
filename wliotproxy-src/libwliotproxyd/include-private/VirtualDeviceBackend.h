#ifndef VIRTUALDEVICEBACKEND_H
#define VIRTUALDEVICEBACKEND_H

#include "wliot/devices/IHighLevelDeviceBackend.h"
#include <QUuid>

class VirtualDeviceBackend
	:public IHighLevelDeviceBackend
{
	Q_OBJECT
public:
	explicit VirtualDeviceBackend(const QUuid &id,const QByteArray &name,const QUuid &typeId,QObject *parent=nullptr);
	void emulateMessageFromDevice(const Message &m);
	void setConnected(bool c);
	virtual bool writeMessageToDevice(const Message &m)override;
	virtual bool isConnected()const override;
	virtual void forceDisconnect()override;
	virtual QByteArray type()const override;
	virtual QByteArray portOrAddress()const override;

signals:
	void messageToDevice(const Message &m);

private:
	Q_INVOKABLE void writeMsgToDeviceQueued(Message m);

public:
	static const QByteArray devType;

private:
	QUuid vDevId;
	QByteArray vDevName;
	QUuid vDevTypeId;
	bool mConnected;
};

#endif // VIRTUALDEVICEBACKEND_H
