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

#include "wliot/client/DevicesList.h"
#include "ServerDeviceBackend.h"

using namespace WLIOT;
using namespace WLIOTClient;

DevicesList::DevicesList(ServerConnection *conn, AllServerCommands *cmds)
	:IDevicesSource(cmds)
{
	commands=cmds;
	srvConn=conn;

	connect(conn,&ServerConnection::preconnected,this,&DevicesList::onServerConnected);
	connect(conn,&ServerConnection::disconnected,this,&DevicesList::onServerDisconnected);
	connect(conn,&ServerConnection::deviceIdentified,this,&DevicesList::onDeviceIdentifiedFromServer);
	connect(conn,&ServerConnection::deviceLost,this,&DevicesList::onDeviceLostFromServer);
	connect(conn,&ServerConnection::deviceStateChanged,this,&DevicesList::onDeviceStateChanged);
	connect(conn,SIGNAL(vdevMsg(QUuid,WLIOT::Message)),this,SLOT(onVDevMsg(QUuid,WLIOT::Message)));
}

QList<TtyPortDescr> DevicesList::ttyPortsList()
{
	QList<TtyPortDescr> retVal;
	commands->devices()->listTty(retVal);
	return retVal;
}

bool DevicesList::identifyTty(const QByteArray &portName)
{
	return commands->devices()->identifyTty(portName);
}

RealDevice* DevicesList::findDevByIdOrName(const QByteArray &idOrName)
{
	QUuid id(idOrName);
	if(!id.isNull())
		return devById(id);
	for(auto d:devices)
		if(d->name()==idOrName)
			return d;
	return nullptr;
}

VirtualDeviceClient* DevicesList::registeredVDev(const QUuid &id)
{
	return virtualDevices.value(id);
}

VirtualDeviceClient* DevicesList::registerVirtualDevice(const QUuid &deviceId,const QByteArray &deviceName,
	const QList<SensorDef> &sensors,const ControlsGroup &controls,const QUuid &typeId,VirtualDeviceCallback *cb)
{
	registeredVDevIds[deviceId]={typeId,deviceName,sensors,controls};
	VirtualDeviceClient *cli=new VirtualDeviceClient(
		srvConn,deviceId,deviceName,typeId,sensors,controls,this);
	cli->setDevEventsCallback(cb);
	virtualDevices[deviceId]=cli;
	if(!commands->devices()->registerVirtualDevice(deviceId,deviceName,typeId))
	{
		delete virtualDevices[deviceId];
		virtualDevices.remove(deviceId);
		return 0;
	}
	return cli;
}

bool DevicesList::identifyTcp(const QByteArray &host)
{
	return commands->devices()->identifyTcp(host);
}

QList<QUuid> DevicesList::identifiedDevices()
{
	return devices.keys();
}

RealDevice* DevicesList::devById(const QUuid &id)
{
	return devices.value(id);
}

void DevicesList::onServerConnected()
{
	qDebug()<<"IotServerDevices::onServerConnected";
	QList<IdentifiedDeviceDescr> devs;
	commands->devices()->listIdentified(devs);
	for(IdentifiedDeviceDescr &d:devs)
		onDeviceIdentifiedFromServer(d.id,d.name,d.typeId);
	for(auto i=registeredVDevIds.begin();i!=registeredVDevIds.end();)
	{
		VirtualDeviceClient *cli=new VirtualDeviceClient(
			srvConn,i.key(),i.value().name,i.value().typeId,i.value().sensors,i.value().controls,this);
		virtualDevices[i.key()]=cli;
		if(!commands->devices()->registerVirtualDevice(i.key(),i.value().name,i.value().typeId))
		{
			i=registeredVDevIds.erase(i);
			delete virtualDevices[i.key()];
			virtualDevices.remove(i.key());
		}
		else ++i;
	}
}

void DevicesList::onServerDisconnected()
{
	auto devsCopy=devices;
	devices.clear();
	for(RealDevice *d:devsCopy)
		delete d;
	for(VirtualDeviceClient *c:virtualDevices)
		c->deleteLater();
	virtualDevices.clear();
}

void DevicesList::onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name,const QUuid &typeId)
{
	if(!devices.contains(id))
	{
		RealDevice *dev=new RealDevice(this);
		devices[id]=dev;
		//TODO type and port name ???
		ServerDeviceBackend *be=new ServerDeviceBackend(srvConn,commands,id,name,typeId,"","",this);
		dev->setBackend(be);
//		connect(dev,&RealDevice::connected,this,&IotServerDevices::onDeviceConnected);
//		connect(dev,&RealDevice::disconnected,this,&IotServerDevices::onDeviceDisconnected);
	}
	else devices[id]->renameDevice(name);
	emit deviceConnected(id);
}

void DevicesList::onDeviceLostFromServer(const QUuid &id)
{
	if(!devices.contains(id))return;
	RealDevice *dev=devices[id];
	((ServerDeviceBackend*)dev->backend())->setDisconnected();
}

void DevicesList::onDeviceStateChanged(const QUuid &id,const QByteArrayList &args)
{
	if(!devices.contains(id))return;
	((ServerDeviceBackend*)devices[id]->backend())->stateChangedFromServer(args);
}

void DevicesList::onVDevMsg(const QUuid &id,const Message &m)
{
	if(virtualDevices.contains(id))
		virtualDevices[id]->onNewMessageFromServer(m);
}
