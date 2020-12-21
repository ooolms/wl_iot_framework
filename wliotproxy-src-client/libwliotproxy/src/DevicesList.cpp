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
#include "wliot/client/VirtualDeviceCallback.h"

using namespace WLIOT;
using namespace WLIOTClient;

DevicesList::DevicesList(ServerConnection *conn, AllServerCommands *cmds)
	:IDevicesSource(cmds)
{
	commands=cmds;
	srvConn=conn;

	connect(conn,&ServerConnection::connectedForInternalUse,this,&DevicesList::onConnected);
	connect(conn,&ServerConnection::disconnected,this,&DevicesList::onDisconnected);
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

VirtualDeviceClient* DevicesList::registeredVirtualDevice(const QUuid &id)
{
	return virtualDevices.value(id);
}

VirtualDeviceClient* DevicesList::registerVirtualDevice(const QUuid &deviceId,const QByteArray &deviceName,
	const QList<SensorDef> &sensors,const ControlsGroup &controls,const QUuid &typeId,VirtualDeviceCallback *cb)
{
	if(registeredVDevIds.contains(deviceId)&&virtualDevices.contains(deviceId))
		disconnectVirtualDevice(deviceId);
	VDevCfg cfg={typeId,deviceName,sensors,controls,cb};
	registeredVDevIds[deviceId]=cfg;
	if(!srvConn->isConnected())return 0;
	return setupVDev(deviceId,cfg);
}

void DevicesList::disconnectVirtualDevice(const QUuid &devId)
{
	registeredVDevIds.remove(devId);
	if(!virtualDevices.contains(devId))return;
	if(!commands->devices()->disconnectVirtualDevice(devId))return;
	VirtualDeviceClient *cli=virtualDevices.take(devId);
	cli->callback->devClient=0;
	delete cli;
}

void DevicesList::reload()
{
	qDebug()<<"WLIOTClient::DevicesList::reload";
	if(!srvConn->isReady())return;
	QList<IdentifiedDeviceDescr> devs;
	commands->devices()->listIdentified(devs);
	QSet<QUuid> newIds;
	for(IdentifiedDeviceDescr &d:devs)
	{
		newIds.insert(d.id);
		if(!devices.contains(d.id))
			onDeviceIdentifiedFromServer(d.id,d.name,d.typeId);
	}
	for(QUuid &id:devices.keys())
	{
		if(!newIds.contains(id))
			onDeviceLostFromServer(id);
	}
}

bool DevicesList::identifyTcp(const QByteArray &host)
{
	return commands->devices()->identifyTcp(host);
}

QList<QUuid> DevicesList::identifiedDevices()
{
	QList<QUuid> ids;
	for(RealDevice *d:devices)
		if(d->isConnected())
			ids.append(d->id());
	return ids;
}

RealDevice* DevicesList::devById(const QUuid &id)
{
	RealDevice *dev=devices.value(id);
	if(dev)return dev;
	dev=new RealDevice(this);
	ServerDeviceBackend *be=new ServerDeviceBackend(srvConn,commands,id,"",QUuid(),"","",this);
	be->setConnected(false);
	dev->setBackend(be);
	return dev;
}

void DevicesList::onConnected()
{
	qDebug()<<"WLIOTClient::DevicesList::onServerConnected";
	if(!srvConn->isReady())return;
	QList<IdentifiedDeviceDescr> devs;
	commands->devices()->listIdentified(devs);
	for(IdentifiedDeviceDescr &d:devs)
		onDeviceIdentifiedFromServer(d.id,d.name,d.typeId);
	for(auto i=registeredVDevIds.begin();i!=registeredVDevIds.end();)
		setupVDev(i.key(),i.value());
}

void DevicesList::onDisconnected()
{
	for(RealDevice *d:devices)
		((ServerDeviceBackend*)d->backend())->setConnected(false);
	for(VirtualDeviceClient *c:virtualDevices)
	{
		c->callback->devClient=0;
		c->deleteLater();
	}
	virtualDevices.clear();
}

void DevicesList::onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name,const QUuid &typeId)
{
	RealDevice *dev=devices.value(id);
	if(!dev)
	{
		dev=new RealDevice(this);
		//TODO type and port name ???
		ServerDeviceBackend *be=new ServerDeviceBackend(srvConn,commands,id,name,typeId,"","",this);
		dev->setBackend(be);
		dev->identify();
		devices[id]=dev;
//		connect(dev,&RealDevice::connected,this,&IotServerDevices::onDeviceConnected);
//		connect(dev,&RealDevice::disconnected,this,&IotServerDevices::onDeviceDisconnected);
	}
	else
	{
		ServerDeviceBackend *be=(ServerDeviceBackend*)dev->backend();
		be->setName(name);
		be->setConnected(true);
		dev->identify();//reset controlsLoaded, sensorsLoaded and stateLoaded flags
	}
	emit deviceConnected(id);
}

void DevicesList::onDeviceLostFromServer(const QUuid &id)
{
	RealDevice *dev=devices.value(id);
	if(!dev)return;
	((ServerDeviceBackend*)dev->backend())->setConnected(false);
	emit deviceDisconnected(id);
}

void DevicesList::onDeviceStateChanged(const QUuid &id,const QByteArrayList &args)
{
	RealDevice *dev=devices.value(id);
	if(!dev)return;
	((ServerDeviceBackend*)dev->backend())->stateChangedFromServer(args);
}

void DevicesList::onVDevMsg(const QUuid &id,const Message &m)
{
	if(virtualDevices.contains(id))
		virtualDevices[id]->onNewMessageFromServer(m);
}

VirtualDeviceClient *DevicesList::setupVDev(const QUuid &devId, const VDevCfg &cfg)
{
	VirtualDeviceClient *cli=new VirtualDeviceClient(
		srvConn,devId,cfg.name,cfg.typeId,cfg.sensors,cfg.controls,this);
	cli->callback=cfg.cb;
	if(cfg.cb)
	{
		cfg.cb->devClient=cli;
		cfg.cb->prepareState(cli->mState);
	}
	virtualDevices[devId]=cli;
	if(!commands->devices()->registerVirtualDevice(devId,cfg.name,cfg.typeId))
	{
		if(cfg.cb)
			cfg.cb->devClient=0;
		delete virtualDevices.take(devId);
		return 0;
	}
	return cli;
}
