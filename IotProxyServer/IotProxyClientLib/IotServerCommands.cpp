/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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
