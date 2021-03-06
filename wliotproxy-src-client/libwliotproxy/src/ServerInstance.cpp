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

#include "wliot/client/ServerInstance.h"

using namespace WLIOT;
using namespace WLIOTClient;

ServerInstance::ServerInstance(QObject *parent)
	:QObject(parent)
{
	conn=new ServerConnection(this);
	mCommands=new AllServerCommands(conn);
	mDevices=new DevicesList(conn,mCommands);
	mStorages=new StoragesDatabase(conn,mCommands);
}

ServerConnection* ServerInstance::connection()
{
	return conn;
}

AllServerCommands* ServerInstance::commands()
{
	return mCommands;
}

DevicesList* ServerInstance::devices()
{
	return mDevices;
}

StoragesDatabase* ServerInstance::storages()
{
	return mStorages;
}

QByteArray ServerInstance::findDevName(const QUuid &devId)
{
	if(!conn->isConnected())
		return QByteArray();
	if(mDevices->devById(devId))
		return mDevices->devById(devId)->name();
	QByteArray name=mStorages->findDevName(devId);
	if(!name.isEmpty())
		return name;
	QByteArrayList names;
	if(!mCommands->devices()->devNames(QList<QUuid>()<<devId,names)||names.isEmpty())
		return QByteArray();
	return names[0];
}
