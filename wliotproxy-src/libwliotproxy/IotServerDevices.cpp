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
	connect(conn,&IotServerConnection::disconnected,this,&IotServerDevices::onServerDisconnected);
	connect(conn,&IotServerConnection::deviceIdentified,this,&IotServerDevices::onDeviceIdentifiedFromServer);
	connect(conn,&IotServerConnection::deviceLost,this,&IotServerDevices::onDeviceLostFromServer);
	connect(conn,&IotServerConnection::deviceStateChanged,this,&IotServerDevices::onDeviceStateChanged);
	connect(conn,&IotServerConnection::vdevMsg,this,&IotServerDevices::onVDevMsg);
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

RealDevice* IotServerDevices::findDevByIdOrName(const QByteArray &idOrName)
{
	QUuid id(idOrName);
	if(!id.isNull())return devById(id);
	for(auto d:devices)
		if(d->name()==idOrName)
			return d;
	return 0;
}

IotServerVirtualDeviceClient* IotServerDevices::registeredVDev(const QUuid &id)
{
	return virtualDevices.value(id);
}

bool IotServerDevices::registerVirtualDevice(
	const QUuid &deviceId,const QByteArray &deviceName,
	const QList<SensorDef> &sensors,const ControlsGroup &controls)
{
	registeredVDevIds[deviceId]={deviceName,sensors,controls};
	QByteArray sensData,contrData;
	SensorDef::dumpToXml(sensData,sensors);
	ControlsGroup::dumpToXml(contrData,controls);
	if(!commands->devices()->registerVirtualDevice(deviceId,deviceName,sensData,contrData))
	{
		registeredVDevIds.remove(deviceId);
		return false;
	}
	IotServerVirtualDeviceClient *cli=new IotServerVirtualDeviceClient(
		srvConn,deviceId,deviceName,sensors,controls,this);
	virtualDevices[deviceId]=cli;
	return true;
}

bool IotServerDevices::identifyTcp(const QByteArray &host)
{
	return commands->devices()->identifyTcp(host);
}

QList<QUuid> IotServerDevices::identifiedDevices()
{
	return devices.keys();
}

RealDevice* IotServerDevices::devById(const QUuid &id)
{
	return devices.value(id);
}

void IotServerDevices::onServerConnected()
{
	QList<IotServerIdentifiedDeviceDescr> devs;
	commands->devices()->listIdentified(devs);
	for(IotServerIdentifiedDeviceDescr &d:devs)
		onDeviceIdentifiedFromServer(d.id,d.name,d.type);
	for(auto i=registeredVDevIds.begin();i!=registeredVDevIds.end();)
	{
		QByteArray sensData,contrData;
		SensorDef::dumpToXml(sensData,i.value().sensors);
		ControlsGroup::dumpToXml(contrData,i.value().controls);
		if(!commands->devices()->registerVirtualDevice(i.key(),i.value().name,sensData,contrData))
			i=registeredVDevIds.erase(i);
		else
		{
			IotServerVirtualDeviceClient *cli=new IotServerVirtualDeviceClient(
				srvConn,i.key(),i.value().name,i.value().sensors,i.value().controls,this);
			virtualDevices[i.key()]=cli;
		}
	}
}

void IotServerDevices::onServerDisconnected()
{
	for(IotServerVirtualDeviceClient *c:virtualDevices)
		delete c;
	virtualDevices.clear();
	for(IotServerDevice *d:devices)
	{
		d->setDisconnected();
		delete d;
	}
	devices.clear();
}

void IotServerDevices::onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name,const QByteArray &type)
{
	if(!devices.contains(id))
	{
		IotServerDevice *dev;
		dev=new IotServerDevice(srvConn,commands,id,name,type,this);
		devices[id]=dev;
		connect(dev,&IotServerDevice::connected,this,&IotServerDevices::onDeviceConnected);
		connect(dev,&IotServerDevice::disconnected,this,&IotServerDevices::onDeviceDisconnected);
	}
	else devices[id]->devName=name;
	emit deviceIdentified(id,name);
}

void IotServerDevices::onDeviceLostFromServer(const QUuid &id)
{
	if(!devices.contains(id))return;
	IotServerDevice *dev=devices[id];
	devices[id]->setDisconnected();
	devices.remove(id);
	delete dev;
}

void IotServerDevices::onDeviceStateChanged(const QUuid &id,const QByteArrayList &args)
{
	if(!devices.contains(id))return;
	devices[id]->stateChangedFromServer(args);
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

void IotServerDevices::onVDevMsg(const QUuid &id,const Message &m)
{
	if(virtualDevices.contains(id))
		virtualDevices[id]->onNewMessageFromServer(m);
}
