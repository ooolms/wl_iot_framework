#include "IotServerCommands.h"

IotServerCommands::IotServerCommands(IotServerConnection *conn,QObject *parent)
	:QObject(parent)
{
	mConfig=new IotServerConfigurationCommands(conn,this);
	mDevices=new IotServerDevicesCommands(conn,this);
	mStorages=new IotServerStoragesCommands(conn,this);
}

IotServerConfigurationCommands* IotServerCommands::config()
{
	return mConfig;
}

IotServerDevicesCommands* IotServerCommands::devices()
{
	return mDevices;
}

IotServerStoragesCommands* IotServerCommands::storages()
{
	return mStorages;
}
