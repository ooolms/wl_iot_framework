#ifndef IOTSERVERDEVICES_H
#define IOTSERVERDEVICES_H

#include <QObject>
#include "IotServerCommands.h"
#include "IotServerConnection.h"
#include "IotServerDevice.h"
#include "IotServerIDevicesSource.h"

class IotServerDevices
	:public IotServerIDevicesSource
{
	Q_OBJECT
public:
	explicit IotServerDevices(IotServerConnection *conn,IotServerCommands *cmds,QObject *parent=nullptr);
	virtual QList<IotServerTtyPortDescr> ttyPortsList()override;
	virtual bool identifyTty(const QByteArray &portName)override;
	virtual bool identifyTcp(const QByteArray &host)override;
	virtual QList<QUuid> identifiedDevices()override;
	virtual ARpcRealDevice *devById(const QUuid &id)override;

signals:
	void deviceIdentified(const QUuid &id,const QByteArray &name);
	void deviceLost(const QUuid &id);

private slots:
	void onServerConnected();
	void onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name);
	void onDeviceLostFromServer(const QUuid &id);
	void onDeviceConnected();
	void onDeviceDisconnected();

private:
	IotServerCommands *commands;
	IotServerConnection *srvConn;
	QMap<QUuid,IotServerDevice*> devices;
};

#endif // IOTSERVERDEVICES_H
