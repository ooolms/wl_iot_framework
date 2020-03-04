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
#include "IotServerDeviceBackend.h"

IotServerDevices::IotServerDevices(IotServerConnection *conn, IotServerCommands *cmds)
	:IotServerIDevicesSource(cmds)
{
	commands=cmds;
	srvConn=conn;

	connect(conn,&IotServerConnection::preconnected,this,&IotServerDevices::onServerConnected);
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
	if(!id.isNull())
		return devById(id);
	for(auto d:devices)
		if(d->name()==idOrName)
			return d;
	return nullptr;
}

IotServerVirtualDeviceClient* IotServerDevices::registeredVDev(const QUuid &id)
{
	return virtualDevices.value(id);
}

bool IotServerDevices::registerVirtualDevice(
	const QUuid &deviceId,const QByteArray &deviceName,
	const QList<SensorDef> &sensors,const ControlsGroup &controls,const QUuid &typeId)
{
	registeredVDevIds[deviceId]={typeId,deviceName,sensors,controls};
	IotServerVirtualDeviceClient *cli=new IotServerVirtualDeviceClient(
		srvConn,deviceId,deviceName,typeId,sensors,controls,this);
	virtualDevices[deviceId]=cli;
	if(!commands->devices()->registerVirtualDevice(deviceId,deviceName,typeId))
	{
		delete virtualDevices[deviceId];
		virtualDevices.remove(deviceId);
		return false;
	}
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
	qDebug()<<"IotServerDevices::onServerConnected";
	QList<IotServerIdentifiedDeviceDescr> devs;
	commands->devices()->listIdentified(devs);
	for(IotServerIdentifiedDeviceDescr &d:devs)
		onDeviceIdentifiedFromServer(d.id,d.name,d.typeId);
	for(auto i=registeredVDevIds.begin();i!=registeredVDevIds.end();)
	{
		IotServerVirtualDeviceClient *cli=new IotServerVirtualDeviceClient(
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

void IotServerDevices::onServerDisconnected()
{
	auto devsCopy=devices;
	devices.clear();
	for(RealDevice *d:devsCopy)
		delete d;
	for(IotServerVirtualDeviceClient *c:virtualDevices)
		c->deleteLater();
	virtualDevices.clear();
}

void IotServerDevices::onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name,const QUuid &typeId)
{
	if(!devices.contains(id))
	{
		RealDevice *dev=new RealDevice(this);
		devices[id]=dev;
		//TODO type and port name ???
		IotServerDeviceBackend *be=new IotServerDeviceBackend(srvConn,commands,id,name,typeId,"","",this);
		dev->setBackend(be);
//		connect(dev,&RealDevice::connected,this,&IotServerDevices::onDeviceConnected);
//		connect(dev,&RealDevice::disconnected,this,&IotServerDevices::onDeviceDisconnected);
	}
	else devices[id]->renameDevice(name);
	emit deviceConnected(id);
}

void IotServerDevices::onDeviceLostFromServer(const QUuid &id)
{
	if(!devices.contains(id))return;
	RealDevice *dev=devices[id];
	((IotServerDeviceBackend*)dev->backend())->setDisconnected();
}

void IotServerDevices::onDeviceStateChanged(const QUuid &id,const QByteArrayList &args)
{
	if(!devices.contains(id))return;
	((IotServerDeviceBackend*)devices[id]->backend())->stateChangedFromServer(args);
}

void IotServerDevices::onVDevMsg(const QUuid &id,const Message &m)
{
	if(virtualDevices.contains(id))
		virtualDevices[id]->onNewMessageFromServer(m);
}
