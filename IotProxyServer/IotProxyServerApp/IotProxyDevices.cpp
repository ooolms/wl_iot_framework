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

#include "IotProxyDevices.h"
#include "IotProxyConfig.h"
#include "IotProxyInstance.h"

IotProxyDevices::IotProxyDevices(QObject *parent)
	:QObject(parent)
{
	watcher.addPath("/dev/");
	connect(&watcher,&QFileSystemWatcher::directoryChanged,
		this,&IotProxyDevices::setupControllers,Qt::QueuedConnection);
	connect(&tcpServer,&ARpcTcpDeviceDetect::newClient,this,&IotProxyDevices::onNewTcpDeviceConnected);
}

void IotProxyDevices::setup()
{
	allTtyUsbDevices=LsTtyUsbDevices::allTtyUsbDevices();
	if(!tcpServer.isServerListening())
	{
		qFatal("Can't start tcp server on port "+QByteArray::number(ARpcConfig::netDevicePort)+": port is busy");
		return;
	}
	setupControllers();
}

ARpcTtyDevice* IotProxyDevices::ttyDeviceByPortName(const QString &portName)
{
	for(ARpcTtyDevice *dev:mTtyDevices)
		if(dev->portName()==portName)
			return dev;
	return 0;
}

ARpcTcpDevice* IotProxyDevices::tcpDeviceByAddress(const QString &address)
{
	for(ARpcTcpDevice *dev:mTcpDevices)
		if(dev->address()==address)
			return dev;
	return 0;
}

ARpcRealDevice* IotProxyDevices::deviceById(const QUuid &id)
{
	if(identifiedDevices.contains(id))
		return identifiedDevices[id];
	return 0;
}

ARpcRealDevice* IotProxyDevices::findDeviceByName(const QByteArray &name)
{
	for(ARpcRealDevice *dev:identifiedDevices)
		if(dev->name()==name)
			return dev;
	return 0;
}

ARpcRealDevice* IotProxyDevices::deviceByIdOrName(const QByteArray &str)
{
	if(str.isEmpty())
		return 0;
	QUuid id(str);
	if(id.isNull())
		return findDeviceByName(str);
	else
		return deviceById(id);
}

ARpcVirtualDevice* IotProxyDevices::virtualDeviceByIdOrName(const QByteArray &str)
{
	if(str.isEmpty())
		return 0;
	QUuid id(str);
	if(id.isNull())
		for(auto d:mVirtualDevices)
			if(d->name()==str)
				return d;
			else
				for(auto d:mVirtualDevices)
					if(d->id()==str)
						return d;
	return 0;
}

const QList<ARpcTtyDevice*>& IotProxyDevices::ttyDevices()
{
	return mTtyDevices;
}

const QList<ARpcTcpDevice*>& IotProxyDevices::tcpDevices()
{
	return mTcpDevices;
}

const QList<ARpcVirtualDevice*>& IotProxyDevices::virtualDevices()
{
	return mVirtualDevices;
}

const QList<ARpcRealDevice *> &IotProxyDevices::hubDevices()
{
	return mHubDevices;
}

bool IotProxyDevices::usbTtyDeviceByPortName(const QString &portName,LsTtyUsbDevices::DeviceInfo &info)
{
	for(auto &i:allTtyUsbDevices)
		if(i.ttyPortName==portName)
		{
			info=i;
			return true;
		}
	return false;
}

ARpcTtyDevice* IotProxyDevices::addTtyDeviceByPortName(const QString &portName)
{
	ARpcTtyDevice *dev=ttyDeviceByPortName(portName);
	if(dev)
		return dev;
	dev=new ARpcTtyDevice(portName,this);
	mTtyDevices.append(dev);
	dev->tryOpen();
	if(dev->isConnected()&&dev->identify())
		onDeviceIdentified(dev);
	connect(dev,&ARpcTtyDevice::rawMessage,this,&IotProxyDevices::devMsgHandler);
	connect(dev,&ARpcTtyDevice::identificationChanged,this,&IotProxyDevices::onTtyDeviceIdentified);
	connect(dev,&ARpcTtyDevice::disconnected,this,&IotProxyDevices::onTtyDeviceDisconnected);
	connect(dev,&ARpcTtyDevice::childDeviceIdentified,this,&IotProxyDevices::onHubChildDeviceIdentified);
	connect(dev,&ARpcTtyDevice::childDeviceLost,this,&IotProxyDevices::onHubChildDeviceLost);
	return dev;
}

ARpcTcpDevice* IotProxyDevices::addTcpDeviceByAddress(const QString &host)
{
	ARpcTcpDevice *dev=tcpDeviceByAddress(host);
	if(dev)
		return dev;
	dev=new ARpcTcpDevice(host,this);
	mTcpDevices.append(dev);
	dev->reconnect();
	if(dev->isConnected()&&dev->identify())
		onDeviceIdentified(dev);
	connect(dev,&ARpcTcpDevice::rawMessage,this,&IotProxyDevices::devMsgHandler);
	connect(dev,&ARpcTcpDevice::identificationChanged,this,&IotProxyDevices::onTcpDeviceIdentified);
	connect(dev,&ARpcTcpDevice::disconnected,this,&IotProxyDevices::onTcpDeviceDisconnected);
	connect(dev,&ARpcTcpDevice::childDeviceIdentified,this,&IotProxyDevices::onHubChildDeviceIdentified);
	connect(dev,&ARpcTcpDevice::childDeviceLost,this,&IotProxyDevices::onHubChildDeviceLost);
	return dev;
}

QList<QUuid> IotProxyDevices::identifiedDevicesIds()
{
	return identifiedDevices.keys();
}

ARpcVirtualDevice* IotProxyDevices::registerVirtualDevice(const QUuid &id,const QByteArray &name,
	const QList<ARpcSensor> &sensors,
	const ARpcControlsGroup &controls)
{
	ARpcVirtualDevice *dev=(ARpcVirtualDevice*)findDevById(id,mVirtualDevices);
	if(dev)
		return dev;
	if(identifiedDevices.contains(id)) //non-virtual device
		return 0;
	dev=new ARpcVirtualDevice(id,name,sensors,controls);
	mVirtualDevices.append(dev);
	connect(dev,&ARpcVirtualDevice::rawMessage,this,&IotProxyDevices::devMsgHandler);
	connect(dev,&ARpcVirtualDevice::identificationChanged,this,&IotProxyDevices::onVirtualDeviceIdentified);
	connect(dev,&ARpcVirtualDevice::childDeviceIdentified,this,&IotProxyDevices::onHubChildDeviceIdentified);
	connect(dev,&ARpcVirtualDevice::childDeviceLost,this,&IotProxyDevices::onHubChildDeviceLost);
	onDeviceIdentified(dev);
	return dev;
}

void IotProxyDevices::devMsgHandler(const ARpcMessage &m)
{
	ARpcRealDevice *dev=qobject_cast<ARpcRealDevice*>(sender());
	if(!dev)
		return;
	if(m.title==ARpcConfig::infoMsg)
		qDebug()<<"Device info message ("<<dev->id()<<":"<<dev->name()<<")"<<m.args.join("|");
}

void IotProxyDevices::onTtyDeviceIdentified()
{
	ARpcTtyDevice *dev=(ARpcTtyDevice*)sender();
	onDeviceIdentified(dev);
}

void IotProxyDevices::onTcpDeviceIdentified()
{
	ARpcTcpDevice *dev=(ARpcTcpDevice*)sender();
	onDeviceIdentified(dev);
}

void IotProxyDevices::onVirtualDeviceIdentified()
{
	ARpcVirtualDevice *dev=(ARpcVirtualDevice*)sender();
	onDeviceIdentified(dev);
}

void IotProxyDevices::onTtyDeviceDisconnected()
{
	ARpcTtyDevice *dev=(ARpcTtyDevice*)sender();
	onDeviceDisconnected(dev);
}

void IotProxyDevices::onTcpDeviceDisconnected()
{
	ARpcTcpDevice *dev=(ARpcTcpDevice*)sender();
	for(auto i=identifiedDevices.begin();i!=identifiedDevices.end();++i)
	{
		if(i.value()==dev)
		{
			qDebug()<<"Tcp device disconnected: "<<i.key();
			identifiedDevices.erase(i);
			emit deviceDisconnected(i.key());
			return;
		}
	}
}

void IotProxyDevices::setupControllers()
{
	for(QString &addr:IotProxyConfig::tcpAddresses)
	{
		if(addr.isEmpty())
			continue;
		addTcpDeviceByAddress(addr);
	}
	QStringList ttyPorts=extractTtyPorts();
	qDebug()<<"Found tty devices matching configuration: "<<ttyPorts;
	for(QString &portName:ttyPorts)
	{
		if(portName.isEmpty())
			continue;
		addTtyDeviceByPortName(portName);
	}
	if(IotProxyConfig::detectTcpDevices)
	{
		tcpServer.startRegularBroadcasting(2000);
		tcpServer.broadcastServerReadyMessage();
	}
	else
		tcpServer.stopRegularBroadcasting();
}

void IotProxyDevices::onNewTcpDeviceConnected(qintptr s,bool &accepted)
{
	accepted=true;
	ARpcTcpDevice *dev=new ARpcTcpDevice(s,this);
	if(!dev->identify())
	{
		delete dev;
		return;
	}
	QUuid newId=dev->id();
	QByteArray newName=dev->name();
	if(findDevById(newId,mTtyDevices))
	{
		delete dev;
		return;//prefer usb over tcp
	}
	ARpcTcpDevice *oldDev=(ARpcTcpDevice*)findDevById(newId,mTcpDevices);
	if(oldDev)
	{
		QTcpSocket *s=dev->takeSocket();
		delete dev;
		dev=oldDev;
		dev->setNewSocket(s,newId,newName);
	}
	else
	{
		mTcpDevices.append(dev);
		connect(dev,&ARpcTcpDevice::rawMessage,this,&IotProxyDevices::devMsgHandler);
		connect(dev,&ARpcTcpDevice::identificationChanged,this,&IotProxyDevices::onTcpDeviceIdentified);
		connect(dev,&ARpcTcpDevice::disconnected,this,&IotProxyDevices::onTcpDeviceDisconnected);
		connect(dev,&ARpcTcpDevice::childDeviceIdentified,this,&IotProxyDevices::onHubChildDeviceIdentified);
		connect(dev,&ARpcTcpDevice::childDeviceLost,this,&IotProxyDevices::onHubChildDeviceLost);
		onDeviceIdentified(dev);
	}
	qDebug()<<"Tcp device connected: "<<dev->address();
}

void IotProxyDevices::onHubChildDeviceIdentified(const QUuid &deviceId)
{
	ARpcRealDevice *dev=(ARpcRealDevice*)sender();
	if(!dev)return;
	ARpcRealDevice *chDev=dev->childDevice(deviceId);
	if(!chDev)return;
	if(!mHubDevices.contains(chDev))
		mHubDevices.append(chDev);
	onDeviceIdentified(chDev);
}

void IotProxyDevices::onHubChildDeviceLost(const QUuid &deviceId)
{
	ARpcRealDevice *dev=(ARpcRealDevice*)sender();
	if(!dev)return;
	ARpcRealDevice *chDev=dev->childDevice(deviceId);
	if(!chDev)return;
	onDeviceDisconnected(chDev);
}

QStringList IotProxyDevices::extractTtyPorts()
{
	QSet<QString> ports;
	QList<QRegExp> portNameRegExps;
	for(QString &pName:IotProxyConfig::ttyPortNames)
		portNameRegExps.append(QRegExp(pName,Qt::CaseSensitive,QRegExp::WildcardUnix));
	QList<LsTtyUsbDevices::DeviceInfo> ttyDevs=LsTtyUsbDevices::allTtyUsbDevices();
	for(auto &dev:ttyDevs)
	{
		bool found=false;
		for(const QRegExp &exp:portNameRegExps)
		{
			if(exp.exactMatch(dev.ttyPortName))
			{
				found=true;
				ports.insert(dev.ttyPortName);
			}
		}
		if(found)
			continue;
		for(auto &vpItem:IotProxyConfig::ttyByVidPid)
		{
			bool dontMatch=false;
			if(vpItem.vid!=dev.vendorId)
				dontMatch=true;
			else if(!vpItem.pid.isEmpty()&&vpItem.pid!=dev.productId)
				dontMatch=true;
			if(!dontMatch)
			{
				ports.insert(dev.ttyPortName);
				break;
			}
		}
	}
	return ports.toList();
}

void IotProxyDevices::onDeviceIdentified(ARpcRealDevice *dev)
{
	for(auto i=identifiedDevices.begin();i!=identifiedDevices.end();++i)
	{
		if(i.value()==dev)
		{
			identifiedDevices.erase(i);
			break;
		}
	}
	if(!dev->isIdentified())
		return;
	identifiedDevices[dev->id()]=dev;
	qDebug()<<"Device identified: "<<dev->name()<<":"<<dev->id();
	QList<DeviceStorageId> ids;
	if(IotProxyInstance::inst().sensorsStorage()->listSensors(ids))
	{
		for(auto &id:ids)
		{
			if(id.deviceId!=dev->id())continue;
			ARpcISensorStorage *stor=IotProxyInstance::inst().sensorsStorage()->existingStorage(id);
			if(stor)stor->setDeviceName(dev->name());
		}
	}
	emit deviceIdentified(dev->id(),dev->name());
	if(dev->isHubDevice())
		dev->identifyHub();
}

void IotProxyDevices::onDeviceDisconnected(ARpcRealDevice *dev)
{
	QUuid id=dev->id();
	if(!identifiedDevices.contains(id)||identifiedDevices[id]!=dev)return;
	identifiedDevices.remove(id);
	if(dev->identifyHub())
	{
		QList<QUuid> ids=dev->childDevices();
		for(auto &id:ids)
		{
			ARpcRealDevice *cd=dev->childDevice(id);
			if(cd)onDeviceDisconnected(cd);
		}
	}
}

void IotProxyDevices::terminate()
{
	for(auto d:mTcpDevices)
		delete d;
	mTcpDevices.clear();
	for(auto d:mTtyDevices)
		delete d;
	mTtyDevices.clear();

}
