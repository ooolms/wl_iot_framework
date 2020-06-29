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
#include "wliot/devices/StdHighLevelDeviceBackend.h"
#include "wliot/devices/HubDeviceBackend.h"

using namespace WLIOT;

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
		qFatal("Can't start tcp server on port %s",
			(QByteArray::number(WLIOTProtocolDefs::netDevicePort)+": port is busy").constData());
//	libusb_hotplug_register_callback(ServerInstance::inst().usbContext(),(libusb_hotplug_event)
//		(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED|LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
//		LIBUSB_HOTPLUG_ENUMERATE,LIBUSB_HOTPLUG_MATCH_ANY,LIBUSB_HOTPLUG_MATCH_ANY,LIBUSB_HOTPLUG_MATCH_ANY,
//		&Devices::onUsbDeviceEventStatic,this,&usbCbHandle);
	setupControllers();
}

RealDevice* Devices::ttyDeviceByPortName(const QString &portName)
{
	for(RealDevice *dev:mTtyDevices)
		if(((SerialDeviceBackend*)dev->backend())->portName()==portName)
			return dev;
	return nullptr;
}

RealDevice* Devices::tcpDeviceByAddress(const QString &address)
{
	for(RealDevice *dev:mTcpOutDevices)
		if(((TcpDeviceBackend*)dev->backend())->address()==address)
			return dev;
	return nullptr;
}

RealDevice* Devices::deviceById(const QUuid &id)
{
	if(id.isNull())return 0;
	if(!identifiedDevices.contains(id))
	{
		RealDevice *dev=new RealDevice(id,ServerInstance::inst().devNames()->deviceName(id),this);
		identifiedDevices[id]=dev;
	}
	return identifiedDevices.value(id);
}

RealDevice* Devices::deviceByIdOrName(const QByteArray &idOrName)
{
	return deviceById(ServerInstance::inst().findDevId(idOrName));
}

//int Devices::onUsbDeviceEventStatic(libusb_context *ctx,
//	libusb_device *device,libusb_hotplug_event event,void *user_data)
//{
//	Q_UNUSED(ctx)
//	if(event==LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
//		return ((Devices*)user_data)->onUsbDeviceAttached(device);
//	else return ((Devices*)user_data)->onUsbDeviceDetached(device);
//}

//int Devices::onUsbDeviceAttached(libusb_device *device)
//{
//	//IMPL
//	return 0;
//}

//int Devices::onUsbDeviceDetached(libusb_device *device)
//{
//	//IMPL
//	return 0;
//}

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

RealDevice* Devices::addTtyDeviceByPortName(const QString &portName)
{
	RealDevice *dev=ttyDeviceByPortName(portName);
	if(dev)
		return dev;
	dev=new RealDevice(this);
	SerialDeviceBackend *be=new SerialDeviceBackend(portName,dev);
	dev->setBackend(new StdHighLevelDeviceBackend(be,dev));
	mTtyDevices.insert(dev);
	be->tryOpen();
	if(dev->isConnected())
	{
		if(dev->isReady()||dev->identify()==RealDevice::OK)
			onDeviceIdentified(dev);
	}
	connect(dev,SIGNAL(newMessageFromDevice(WLIOT::Message)),this,SLOT(onDeviceMessage(WLIOT::Message)));
	connect(dev,&RealDevice::identified,this,&Devices::onTtyDeviceIdentified,Qt::QueuedConnection);
	connect(dev,&RealDevice::disconnected,this,&Devices::onTtyDeviceDisconnected,Qt::QueuedConnection);
	connect(dev,&RealDevice::childDeviceIdentified,this,&Devices::onHubChildDeviceIdentified);
	connect(dev,SIGNAL(stateChanged(QByteArrayList)),this,SLOT(onDevStateChanged(QByteArrayList)));
	return dev;
}

RealDevice* Devices::addTcpDeviceByAddress(const QString &host)
{
	RealDevice *dev=tcpDeviceByAddress(host);
	if(dev)
		return dev;
	dev=new RealDevice(this);
	TcpDeviceBackend *be=new TcpDeviceBackend(host,dev);
	dev->setBackend(new StdHighLevelDeviceBackend(be,dev));
	mTcpOutDevices.insert(dev);
	be->waitForConnected();
	if(dev->isConnected())
	{
		if(dev->isReady()||dev->identify()==RealDevice::OK)
			onDeviceIdentified(dev);
	}
	connect(dev,SIGNAL(newMessageFromDevice(WLIOT::Message)),this,SLOT(onDeviceMessage(WLIOT::Message)));
	connect(dev,&RealDevice::identified,this,&Devices::onTcpDeviceIdentified,Qt::QueuedConnection);
	connect(dev,&RealDevice::disconnected,this,&Devices::onTcpDeviceDisconnected,Qt::QueuedConnection);
	connect(dev,&RealDevice::childDeviceIdentified,this,&Devices::onHubChildDeviceIdentified);
	connect(dev,SIGNAL(stateChanged(QByteArrayList)),this,SLOT(onDevStateChanged(QByteArrayList)));
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
	if(identifiedDevices.contains(id))//non-virtual device
		return nullptr;
	dev=new VirtualDevice(id,name,typeId);
	mVirtualDevices.insert(dev);
	connect(dev,SIGNAL(newMessageFromDevice(WLIOT::Message)),this,SLOT(onDeviceMessage(WLIOT::Message)));
	connect(dev,&VirtualDevice::identified,this,&Devices::onVirtualDeviceIdentified,Qt::QueuedConnection);
	connect(dev,&VirtualDevice::connected,this,&Devices::onVirtualDeviceIdentified,Qt::QueuedConnection);
	connect(dev,&VirtualDevice::disconnected,this,&Devices::onVirtualDeviceDisconnected,Qt::QueuedConnection);
	connect(dev,&VirtualDevice::childDeviceIdentified,this,&Devices::onHubChildDeviceIdentified);
	connect(dev,SIGNAL(stateChanged(QByteArrayList)),this,SLOT(onDevStateChanged(QByteArrayList)));
	onDeviceIdentified(dev);
	return dev;
}

void Devices::onDeviceMessage(const WLIOT::Message &m)
{
	RealDevice *dev=qobject_cast<RealDevice*>(sender());
	if(!dev||!dev->isReady())
		return;
	if(m.title==WLIOTProtocolDefs::infoMsg)
		qDebug()<<"Device info message ("<<dev->id()<<":"<<dev->name()<<")"<<m.args.join("|");
}

void Devices::onTtyDeviceIdentified()
{
	RealDevice *dev=(RealDevice*)sender();
	onDeviceIdentified(dev);
}

void Devices::onTcpDeviceIdentified()
{
	RealDevice *dev=(RealDevice*)sender();
	onDeviceIdentified(dev);
}

void Devices::onVirtualDeviceIdentified()
{
	VirtualDevice *dev=(VirtualDevice*)sender();
	onDeviceIdentified(dev);
}

void Devices::onTtyDeviceDisconnected()
{
	RealDevice *dev=(RealDevice*)sender();
	qDebug()<<"Tty device disconnected: "<<dev->id()<<":"<<dev->name();
	onDeviceDisconnected(dev);
	mTtyDevices.remove(dev);
	dev->deleteLater();
}

void Devices::onTcpDeviceDisconnected()
{
	RealDevice *dev=(RealDevice*)sender();
	qDebug()<<"Tcp device disconnected: "<<dev->id()<<":"<<dev->name();
	onDeviceDisconnected(dev);
	mTcpInDevices.remove(dev);
	mTcpOutDevices.remove(dev);
	dev->deleteLater();
}

void Devices::onVirtualDeviceDisconnected()
{
	VirtualDevice *dev=(VirtualDevice*)sender();
	qDebug()<<"Virtual device disconnected: "<<dev->id()<<":"<<dev->name();
	onDeviceDisconnected(dev);
	mVirtualDevices.remove(dev);
	dev->deleteLater();
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
		tcpServer.startRegularBroadcasting(10000);
		tcpServer.broadcastServerReadyMessage();
	}
	else
		tcpServer.stopRegularBroadcasting();
}

void Devices::onNewTcpDeviceConnected(qintptr s,bool &accepted)
{
	accepted=true;
	RealDevice *dev=new RealDevice(this);
	TcpDeviceBackend *be=new TcpDeviceBackend(s,dev);
	dev->setBackend(new StdHighLevelDeviceBackend(be,dev));
	if(dev->id().isNull()&&dev->identify()!=RealDevice::OK)
	{
		delete dev;
		return;
	}
	QUuid newId=dev->id();
	QByteArray newName=dev->name();
	if(findDevById(newId,mTtyDevices))
	{
		delete dev;
		return;//prefer usb/tty over tcp
	}
	RealDevice *oldDev=(RealDevice*)findDevById(newId,mTcpInDevices);
	if(!oldDev)
		oldDev=(RealDevice*)findDevById(newId,mTcpOutDevices);
	if(oldDev)
	{
		oldDev->setBackend(dev->takeBackend());
		dev->deleteLater();
	}
	else
	{
		mTcpInDevices.insert(dev);
		connect(dev,SIGNAL(newMessageFromDevice(WLIOT::Message)),this,SLOT(onDeviceMessage(WLIOT::Message)));
		connect(dev,&RealDevice::identified,this,&Devices::onTcpDeviceIdentified,Qt::QueuedConnection);
		connect(dev,&RealDevice::disconnected,this,&Devices::onTcpDeviceDisconnected,Qt::QueuedConnection);
		connect(dev,&RealDevice::childDeviceIdentified,this,&Devices::onHubChildDeviceIdentified);
		connect(dev,SIGNAL(stateChanged(QByteArrayList)),this,SLOT(onDevStateChanged(QByteArrayList)));
		onDeviceIdentified(dev);
	}
	qDebug()<<"Tcp device connected: "<<be->address();
}

void Devices::onHubChildDeviceIdentified(const QUuid &deviceId)
{
	RealDevice *dev=(RealDevice*)sender();
	if(!dev)return;
	HubDeviceBackend *chDevBe=dev->childDevice(deviceId);
	if(!chDevBe)return;
	RealDevice *chDev=identifiedDevices.value(deviceId);
	if(chDev&&!chDev->backend())
	{
		chDev->setBackend(chDevBe);
		onDeviceIdentified(chDev);
	}
	else
	{
		chDev=new RealDevice(this);
		chDev->setBackend(chDevBe);
		onDeviceIdentified(chDev);
	}
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
	if(!dev->isReady())
		return;
	identifiedDevices[dev->id()]=dev;
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
	ServerInstance::inst().devNames()->onDeviceIdentified(dev->id(),dev->name());
	dev->renameDevice(ServerInstance::inst().devNames()->deviceName(dev->id()));
	qDebug()<<"Device identified: "<<dev->name()<<":"<<dev->id();
	emit deviceIdentified(dev->id(),dev->name(),dev->classId());
	if(dev->isHubDevice())
		dev->identifyHub();
}

void Devices::onDeviceDisconnected(RealDevice *dev)
{
	QUuid id=dev->id();
	if(!identifiedDevices.contains(id)||identifiedDevices[id]!=dev)return;
	identifiedDevices.remove(id);
	emit deviceDisconnected(dev->id());
}

void Devices::terminate()
{
	for(auto d:QSet<RealDevice*>(mTcpInDevices))
		delete d;
	mTcpInDevices.clear();
	for(auto d:QSet<RealDevice*>(mTcpOutDevices))
		delete d;
	mTcpOutDevices.clear();
	for(auto d:QSet<RealDevice*>(mTtyDevices))
		delete d;
	mTtyDevices.clear();
}
