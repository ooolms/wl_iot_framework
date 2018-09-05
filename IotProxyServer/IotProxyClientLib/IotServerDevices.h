#ifndef IOTSERVERDEVICES_H
#define IOTSERVERDEVICES_H

#include <QObject>
#include "IotServerCommands.h"
#include "IotServerConnection.h"
#include "IotServerDevice.h"

class IotServerDevices
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerDevices(IotServerCommands *cmds,IotServerConnection *conn,QObject *parent=nullptr);
	QList<IotServerTtyPortDescr> ttyPortsList();
	bool identifyTty(const QByteArray &portName);
	QList<QUuid> identifiedDevices();
	IotServerDevice *devById(const QUuid &id);

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
