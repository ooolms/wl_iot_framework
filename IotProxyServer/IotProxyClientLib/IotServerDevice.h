#ifndef IOTSERVERDEVICE_H
#define IOTSERVERDEVICE_H

#include "ARpcBase/ARpcRealDevice.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"

class IotServerDevice
	:public ARpcRealDevice
{
	Q_OBJECT
public:
	explicit IotServerDevice(IotServerConnection *conn,IotServerCommands *cmds,const QUuid &id,
		const QByteArray &name,QObject *parent=nullptr);
	void setDeviceConnected(bool c);

public:
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

private slots:
	void processMessages();
	void onServerConnectionChanged();

private:
	void processMessage(const ARpcMessage &m);

private:
	IotServerConnection *srvConn;
	IotServerCommands *commands;
	bool deviceConnectedFlag;
	QUuid devId;
	QByteArray devName;
	QList<ARpcMessage> messagesToDevice;
};

#endif // IOTSERVERDEVICE_H
