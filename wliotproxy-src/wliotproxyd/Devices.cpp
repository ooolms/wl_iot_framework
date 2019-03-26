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

#include "Devices.h"
#include "MainServerConfig.h"
#include "ServerInstance.h"

Devices::Devices(QObject *parent)
	:QObject(parent)
{
	watcher.addPath("/dev/");
	connect(&watcher,&QFileSystemWatcher::directoryChanged,
		this,&Devices::setupControllers,Qt::QueuedConnection);
	connect(&tcpServer,&TcpDeviceDetect::newClient,this,&Devices::onNewTcpDeviceConnected);
}

Devices::~Devices()
{
//	libusb_hotplug_deregister_callback(ServerInstance::inst().usbContext(),usbCbHandle);
}

void Devices::setup()
{
	allTtyUsbDevices=LsTtyUsbDevices::allTtyUsbDevices();
	if(!tcpServer.isServerListening())
	{
		qFatal("Can't start tcp server on port "+QByteArray::number(WLIOTProtocolDefs::netDevicePort)+": port is busy");
		return;
	}
//	libusb_hotplug_register_callback(ServerInstance::inst().usbContext(),(libusb_hotplug_event)
//		(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED|LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
//		LIBUSB_HOTPLUG_ENUMERATE,LIBUSB_HOTPLUG_MATCH_ANY,LIBUSB_HOTPLUG_MATCH_ANY,LIBUSB_HOTPLUG_MATCH_ANY,
//		&Devices::onUsbDeviceEventStatic,this,&usbCbHandle);
	setupControllers();
}

SerialDevice* Devices::ttyDeviceByPortName(const QString &portName)
{
	for(SerialDevice *dev:mTtyDevices)
		if(dev->portName()==portName)
			return dev;
	return 0;
}

TcpDevice* Devices::tcpDeviceByAddress(const QString &address)
{
	for(TcpDevice *dev:mTcpDevices)
		if(dev->address()==address)
			return dev;
	return 0;
}

RealDevice* Devices::deviceById(const QUuid &id)
{
	if(identifiedDevices.contains(id))
		return identifiedDevices[id];
	return 0;
}

RealDevice* Devices::findDeviceByName(const QByteArray &name)
{
	RealDevice *dev=0;
	int count=0;
	for(RealDevice *d:identifiedDevices)
	{
		if(d->name()==name)
		{
			dev=d;
			++count;
		}
	}
	if(count==1)return dev;
	return 0;
}

int Devices::onUsbDeviceEventStatic(libusb_context *ctx,
	libusb_device *device,libusb_hotplug_event event,void *user_data)
{
	Q_UNUSED(ctx)
	if(event==LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
		return ((Devices*)user_data)->onUsbDeviceAttached(device);
	else return ((Devices*)user_data)->onUsbDeviceDetached(device);
}

int Devices::onUsbDeviceAttached(libusb_device *device)
{
	//IMPL
	return 0;
}

int Devices::onUsbDeviceDetached(libusb_device *device)
{
	//IMPL
	return 0;
}

RealDevice* Devices::deviceByIdOrName(const QByteArray &str)
{
	if(str.isEmpty())
		return 0;
	QUuid id(str);
	if(id.isNull())
		return findDeviceByName(str);
	else
		return deviceById(id);
}

VirtualDevice* Devices::virtualDeviceByIdOrName(const QByteArray &str)
{
	if(str.isEmpty())
		return 0;
	QUuid id(str);
	if(id.isNull())
	{
		for(auto d:mVirtualDevices)
			if(d->name()==str)
				return d;
	}
	else
	{
		for(auto d:mVirtualDevices)
			if(d->id()==id)
				return d;
	}
	return 0;
}

const QList<SerialDevice*>& Devices::ttyDevices()
{
	return mTtyDevices;
}

const QList<TcpDevice*>& Devices::tcpDevices()
{
	return mTcpDevices;
}

const QList<VirtualDevice*>& Devices::virtualDevices()
{
	return mVirtualDevices;
}

const QList<RealDevice *> &Devices::hubDevices()
{
	return mHubDevices;
}

bool Devices::usbTtyDeviceByPortName(const QString &portName,LsTtyUsbDevices::DeviceInfo &info)
{
	for(auto &i:allTtyUsbDevices)
		if(i.ttyPortName==portName)
		{
			info=i;
			return true;
		}
	return false;
}

SerialDevice* Devices::addTtyDeviceByPortName(const QString &portName)
{
	SerialDevice *dev=ttyDeviceByPortName(portName);
	if(dev)
		return dev;
	dev=new SerialDevice(portName,this);
	mTtyDevices.append(dev);
	dev->tryOpen();
	if(dev->isConnected())
	{
		if(dev->isIdentified()||dev->identify()==RealDevice::OK)
			onDeviceIdentified(dev);
	}
	connect(dev,&SerialDevice::newMessageFromDevice,this,&Devices::onDeviceMessage);
	connect(dev,&SerialDevice::identificationChanged,this,&Devices::onTtyDeviceIdentified);
	connect(dev,&SerialDevice::disconnected,this,&Devices::onTtyDeviceDisconnected);
	connect(dev,&SerialDevice::childDeviceIdentified,this,&Devices::onHubChildDeviceIdentified);
	connect(dev,&SerialDevice::childDeviceLost,this,&Devices::onHubChildDeviceLost);
	connect(dev,&SerialDevice::stateChanged,this,&Devices::onDevStateChanged);
	return dev;
}

TcpDevice* Devices::addTcpDeviceByAddress(const QString &host)
{
	TcpDevice *dev=tcpDeviceByAddress(host);
	if(dev)
		return dev;
	dev=new TcpDevice(host,this);
	mTcpDevices.append(dev);
	if(dev->isConnected())
	{
		if(dev->isIdentified()||dev->identify()==RealDevice::OK)
			onDeviceIdentified(dev);
	}
	connect(dev,&TcpDevice::newMessageFromDevice,this,&Devices::onDeviceMessage);
	connect(dev,&TcpDevice::identificationChanged,this,&Devices::onTcpDeviceIdentified);
	connect(dev,&TcpDevice::disconnected,this,&Devices::onTcpDeviceDisconnected);
	connect(dev,&TcpDevice::childDeviceIdentified,this,&Devices::onHubChildDeviceIdentified);
	connect(dev,&TcpDevice::childDeviceLost,this,&Devices::onHubChildDeviceLost);
	connect(dev,&TcpDevice::stateChanged,this,&Devices::onDevStateChanged);
	return dev;
}

QList<QUuid> Devices::identifiedDevicesIds()
{
	return identifiedDevices.keys();
}

VirtualDevice* Devices::registerVirtualDevice(const QUuid &id,const QByteArray &name,const QUuid &typeId)
{
	VirtualDevice *dev=(VirtualDevice*)findDevById(id,mVirtualDevices);
	if(dev)
	{
		//emit deviceIdentified(dev->id(),dev->name(),dev->deviceType());
		return dev;
	}
	if(identifiedDevices.contains(id)) //non-virtual device
		return 0;
	dev=new VirtualDevice(id,name,typeId);
	mVirtualDevices.append(dev);
	dev->setConnected(true);
	connect(dev,&VirtualDevice::newMessageFromDevice,this,&Devices::onDeviceMessage);
	connect(dev,&VirtualDevice::identificationChanged,this,&Devices::onVirtualDeviceIdentified);
	connect(dev,&VirtualDevice::childDeviceIdentified,this,&Devices::onHubChildDeviceIdentified);
	connect(dev,&VirtualDevice::childDeviceLost,this,&Devices::onHubChildDeviceLost);
	connect(dev,&VirtualDevice::stateChanged,this,&Devices::onDevStateChanged);
	connect(dev,&VirtualDevice::disconnected,this,&Devices::onVirtualDeviceDisconnected);
	onDeviceIdentified(dev);
	return dev;
}

void Devices::onDeviceMessage(const Message &m)
{
	RealDevice *dev=qobject_cast<RealDevice*>(sender());
	if(!dev||!dev->isIdentified())
		return;
	if(m.title==WLIOTProtocolDefs::infoMsg)
		qDebug()<<"Device info message ("<<dev->id()<<":"<<dev->name()<<")"<<m.args.join("|");
}

void Devices::onTtyDeviceIdentified()
{
	SerialDevice *dev=(SerialDevice*)sender();
	onDeviceIdentified(dev);
}

void Devices::onTcpDeviceIdentified()
{
	TcpDevice *dev=(TcpDevice*)sender();
	onDeviceIdentified(dev);
}

void Devices::onVirtualDeviceIdentified()
{
	VirtualDevice *dev=(VirtualDevice*)sender();
	onDeviceIdentified(dev);
}

void Devices::onTtyDeviceDisconnected()
{
	SerialDevice *dev=(SerialDevice*)sender();
	qDebug()<<"Tty device disconnected: "<<dev->id()<<":"<<dev->name();
	onDeviceDisconnected(dev);
	mTtyDevices.removeOne(dev);
	delete dev;
}

void Devices::onTcpDeviceDisconnected()
{
	TcpDevice *dev=(TcpDevice*)sender();
	qDebug()<<"Tcp device disconnected: "<<dev->id()<<":"<<dev->name();
	onDeviceDisconnected(dev);
	mTcpDevices.removeOne(dev);
	delete dev;
}

void Devices::onVirtualDeviceDisconnected()
{
	VirtualDevice *dev=(VirtualDevice*)sender();
	qDebug()<<"Virtual device disconnected: "<<dev->id()<<":"<<dev->name();
	onDeviceDisconnected(dev);
	mVirtualDevices.removeOne(dev);
	delete dev;
}

void Devices::setupControllers()
{
	for(QString &addr:MainServerConfig::tcpAddresses)
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
	if(MainServerConfig::detectTcpDevices)
	{
		tcpServer.startRegularBroadcasting(2000);
		tcpServer.broadcastServerReadyMessage();
	}
	else
		tcpServer.stopRegularBroadcasting();
}

void Devices::onNewTcpDeviceConnected(qintptr s,bool &accepted)
{
	accepted=true;
	TcpDevice *dev=new TcpDevice(s,this);
	if(dev->identify()!=RealDevice::OK)
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
	TcpDevice *oldDev=(TcpDevice*)findDevById(newId,mTcpDevices);
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
		connect(dev,&TcpDevice::newMessageFromDevice,this,&Devices::onDeviceMessage);
		connect(dev,&TcpDevice::identificationChanged,this,&Devices::onTcpDeviceIdentified);
		connect(dev,&TcpDevice::disconnected,this,&Devices::onTcpDeviceDisconnected);
		connect(dev,&TcpDevice::childDeviceIdentified,this,&Devices::onHubChildDeviceIdentified);
		connect(dev,&TcpDevice::childDeviceLost,this,&Devices::onHubChildDeviceLost);
		connect(dev,&TcpDevice::stateChanged,this,&Devices::onDevStateChanged);
		onDeviceIdentified(dev);
	}
	qDebug()<<"Tcp device connected: "<<dev->address();
}

void Devices::onHubChildDeviceIdentified(const QUuid &deviceId)
{
	RealDevice *dev=(RealDevice*)sender();
	if(!dev)return;
	RealDevice *chDev=dev->childDevice(deviceId);
	if(!chDev)return;
	if(!mHubDevices.contains(chDev))
		mHubDevices.append(chDev);
	onDeviceIdentified(chDev);
}

void Devices::onHubChildDeviceLost(const QUuid &deviceId)
{
	RealDevice *dev=(RealDevice*)sender();
	if(!dev)return;
	RealDevice *chDev=dev->childDevice(deviceId);
	if(!chDev)return;
	onDeviceDisconnected(chDev);
}

void Devices::onDevStateChanged(const QByteArrayList &args)
{
	RealDevice *dev=(RealDevice*)sender();
	qDebug()<<"Device state changed: "<<dev->id()<<": "<<args;
	emit deviceStateChanged(dev->id(),args);
}

QStringList Devices::extractTtyPorts()
{
	QSet<QString> ports;
	QList<QRegExp> portNameRegExps;
	for(QString &pName:MainServerConfig::ttyPortNames)
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
		for(auto &vpItem:MainServerConfig::ttyByVidPid)
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

void Devices::onDeviceIdentified(RealDevice *dev)
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
	QList<StorageId> ids;
	if(ServerInstance::inst().storages()->listStorages(ids))
	{
		for(auto &id:ids)
		{
			if(id.deviceId!=dev->id())continue;
			BaseFSSensorStorage *stor=(BaseFSSensorStorage*)
				ServerInstance::inst().storages()->existingStorage(id);
			if(stor)stor->setDeviceName(dev->name());
		}
	}
	emit deviceIdentified(dev->id(),dev->name(),dev->typeId());
	if(dev->isHubDevice())
		dev->identifyHub();
}

void Devices::onDeviceDisconnected(RealDevice *dev)
{
	QUuid id=dev->id();
	if(!identifiedDevices.contains(id)||identifiedDevices[id]!=dev)return;
	identifiedDevices.remove(id);
	if(dev->isHubDevice())
	{
		QList<QUuid> ids=dev->childDevices();
		for(auto &id:ids)
		{
			RealDevice *cd=dev->childDevice(id);
			if(cd)onDeviceDisconnected(cd);
		}
	}
	emit deviceDisconnected(dev->id());
}

void Devices::terminate()
{
	for(auto d:mTcpDevices)
		delete d;
	mTcpDevices.clear();
	for(auto d:mTtyDevices)
		delete d;
	mTtyDevices.clear();

}
