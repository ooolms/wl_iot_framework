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

#include "IotServerDevices.h"

IotServerDevices::IotServerDevices(IotServerConnection *conn,IotServerCommands *cmds,QObject *parent)
	:IotServerIDevicesSource(parent)
{
	commands=cmds;
	srvConn=conn;

	connect(conn,&IotServerConnection::connected,this,&IotServerDevices::onServerConnected);
	connect(conn,&IotServerConnection::deviceIdentified,this,&IotServerDevices::onDeviceIdentifiedFromServer);
	connect(conn,&IotServerConnection::deviceLost,this,&IotServerDevices::onDeviceLostFromServer);
}

QList<IotServerTtyPortDescr> IotServerDevices::ttyPortsList()
{
	QList<IotServerTtyPortDescr> retVal;
	commands->devices()->listTty(retVal);
	return retVal;
}

bool IotServerDevices::identifyTty(const QByteArray &portName)
{
	return commands->devices()->identifyTty(portName);
}

bool IotServerDevices::identifyTcp(const QByteArray &host)
{
	return commands->devices()->identifyTcp(host);
}

QList<QUuid> IotServerDevices::identifiedDevices()
{
	return devices.keys();
}

ARpcRealDevice* IotServerDevices::devById(const QUuid &id)
{
	return devices.value(id);
}

void IotServerDevices::onServerConnected()
{
	QList<IotServerIdentifiedDeviceDescr> devs;
	commands->devices()->listIdentified(devs);
	QSet<QUuid> ids;
	for(IotServerIdentifiedDeviceDescr &d:devs)
	{
		ids.insert(d.id);
		onDeviceIdentifiedFromServer(d.id,d.name);
	}
	for(auto d:devices)
	{
		if(!ids.contains(d->id()))
			d->setDeviceConnected(false);
	}
}

void IotServerDevices::onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name)
{
	if(!devices.contains(id))
	{
		IotServerDevice *dev=new IotServerDevice(srvConn,commands,id,name,this);
		devices[id]=dev;
		connect(dev,&IotServerDevice::connected,this,&IotServerDevices::onDeviceConnected);
		connect(dev,&IotServerDevice::disconnected,this,&IotServerDevices::onDeviceDisconnected);
		dev->setDeviceConnected(true);
	}
	else devices[id]->setDeviceConnected(true);
}

void IotServerDevices::onDeviceLostFromServer(const QUuid &id)
{
	if(!devices.contains(id))return;
	devices[id]->setDeviceConnected(false);
}

void IotServerDevices::onDeviceConnected()
{
	IotServerDevice *dev=(IotServerDevice*)sender();
	emit deviceIdentified(dev->id(),dev->name());
}

void IotServerDevices::onDeviceDisconnected()
{
	IotServerDevice *dev=(IotServerDevice*)sender();
	emit deviceLost(dev->id());
}
