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
	connect(conn,&IotServerConnection::processVirtualDeviceCommand,this,&IotServerDevices::onProcessVDeviceCommand,
		Qt::DirectConnection);
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

ARpcRealDevice* IotServerDevices::findDevByIdOrName(const QByteArray &idOrName)
{
	QUuid id(idOrName);
	if(!id.isNull())return devById(id);
	for(auto d:devices)
		if(d->name()==idOrName)
			return d;
	return 0;
}

IotServerVirtualDevice* IotServerDevices::virtualDevById(const QUuid &id)
{
	return virtualDevices.value(id);
}

bool IotServerDevices::registerVirtualDevice(
	const QUuid &deviceId,const QByteArray &deviceName,
	const QList<ARpcSensorDef> &sensors,const ARpcControlsGroup &controls)
{
	registeredVDevIds[deviceId]={deviceName,sensors,controls};
	QByteArray sensData,contrData;
	ARpcSensorDef::dumpToXml(sensData,sensors);
	ARpcControlsGroup::dumpToXml(contrData,controls);
	return commands->devices()->registerVirtualDevice(deviceId,deviceName,sensData,contrData);
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
		if(registeredVDevIds.contains(d.id))
		{
			VDevCfg &cfg=registeredVDevIds[d.id];
			QByteArray sensData,contrData;
			ARpcSensorDef::dumpToXml(sensData,cfg.sensors);
			ARpcControlsGroup::dumpToXml(contrData,cfg.controls);
			commands->devices()->registerVirtualDevice(d.id,cfg.name,sensData,contrData);
		}
		else onDeviceIdentifiedFromServer(d.id,d.name,d.type);
	}
	for(auto d:devices)
	{
		if(!ids.contains(d->id()))
			d->setDeviceConnected(false);
	}
}

void IotServerDevices::onDeviceIdentifiedFromServer(const QUuid &id,const QByteArray &name,const QByteArray &type)
{
	if(!devices.contains(id))
	{
		IotServerDevice *dev;
		if(type=="virtual"&&registeredVDevIds.contains(id))
		{
			dev=new IotServerVirtualDevice(srvConn,commands,id,name,type,this);
			virtualDevices[id]=(IotServerVirtualDevice*)dev;
		}
		else dev=new IotServerDevice(srvConn,commands,id,name,type,this);
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

void IotServerDevices::onDeviceStateChanged(const QUuid &id,const QByteArrayList &args)
{
	if(!devices.contains(id))return;
	devices[id]->stateChangedFromServer(args);
}

void IotServerDevices::onDeviceConnected()
{
	IotServerDevice *dev=(IotServerDevice*)sender();
	emit deviceIdentified(dev->id(),dev->name(),dev->deviceType());
}

void IotServerDevices::onDeviceDisconnected()
{
	IotServerDevice *dev=(IotServerDevice*)sender();
	emit deviceLost(dev->id());
}

void IotServerDevices::onProcessVDeviceCommand(
	const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal)
{
	if(virtualDevices.contains(devId))
		emit virtualDevices[devId]->processVirtualDeviceCommand(cmd,args,ok,retVal);
}
