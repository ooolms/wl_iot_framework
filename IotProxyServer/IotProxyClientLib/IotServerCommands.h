#ifndef IOTSERVERCOMMANDS_H
#define IOTSERVERCOMMANDS_H

#include <QObject>
#include "IotServerDevicesCommands.h"
#include "IotServerStoragesCommands.h"
#include "IotServerConfigurationCommands.h"

class IotServerCommands
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerCommands(IotServerConnection *conn,QObject *parent=nullptr);
	IotServerConfigurationCommands* config();
	IotServerDevicesCommands* devices();
	IotServerStoragesCommands* storages();

private:
	IotServerConfigurationCommands *mConfig;
	IotServerDevicesCommands *mDevices;
	IotServerStoragesCommands *mStorages;
};

#endif // IOTSERVERCOMMANDS_H
