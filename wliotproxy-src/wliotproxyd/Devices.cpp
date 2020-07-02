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
#include "DeviceTypesPriority.h"
#include "MainServerConfig.h"
#include "ServerInstance.h"
#include "wliot/devices/HubDeviceBackend.h"
#include "wliot/devices/SerialDeviceBackend.h"
#include "wliot/devices/TcpDeviceBackend.h"
#include "wliot/devices/TcpSslDeviceBackend.h"

using namespace WLIOT;

//CRIT отключающиеся out устройства
/*
 * обработка списка устройств, к которым сервер должен подключаться сам:
 * - последовательные порты: могут появляться и пропадать (usb), но могут и просто отваливаться устройства, а
 * порт оставаться, если он аппаратный
 * - tcp устройства: если отваливается, надо автоматически переподключаться по таймеру
 * РЕШЕНИЕ:
 * 1. сделать класс, который по таймеру пытается с помощью функции addOutDevice подключить устройства;
 * 2. в функции checkDevicesFromConfig (или addOutDevice ???) добавлять устройства, к которым подключиться не удалось,
 * в список первого класса (где таймер)
*/

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
	terminate();
}

void Devices::setup()
{
	allTtyUsbDeviceInfos=LsTtyUsbDevices::allTtyUsbDevices();
	if(!tcpServer.isServerListening())
		qFatal("Can't start tcp server on port %s",
			(QByteArray::number(WLIOTProtocolDefs::netDevicePort)+": port is busy").constData());
//	libusb_hotplug_register_callback(ServerInstance::inst().usbContext(),(libusb_hotplug_event)
//		(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED|LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
//		LIBUSB_HOTPLUG_ENUMERATE,LIBUSB_HOTPLUG_MATCH_ANY,LIBUSB_HOTPLUG_MATCH_ANY,LIBUSB_HOTPLUG_MATCH_ANY,
//		&Devices::onUsbDeviceEventStatic,this,&usbCbHandle);
	setupControllers();
}

IHighLevelDeviceBackend* Devices::ttyBackendByPortName(const QString &portName)
{
	return mOutBackendsByType[SerialDeviceBackend::mBackendType].value(portName);
}

IHighLevelDeviceBackend* Devices::tcpBackendByAddress(const QString &address)
{
	return mOutBackendsByType[TcpDeviceBackend::mBackendType].value(address);
}

IHighLevelDeviceBackend *Devices::tcpSslBackendByAddress(const QString &address)
{
	return mOutBackendsByType[TcpSslDeviceBackend::mBackendType].value(address);
}

RealDevice* Devices::deviceById(const QUuid &id)
{
	if(id.isNull())return 0;
	if(!mIdentifiedDevices.contains(id))
	{
		RealDevice *dev=new RealDevice(id,ServerInstance::inst().devNames()->deviceName(id),this);
		mIdentifiedDevices[id]=dev;
		return dev;
	}
	return mIdentifiedDevices.value(id);
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
	if(!allTtyUsbDeviceInfos.contains(portName))return false;
	info=allTtyUsbDeviceInfos.value(portName);
	return true;
}

void Devices::addOutDevice(const QByteArray &backendType,const QString &hwAddress,const QString &connString)
{
	if(mOutBackendsByType[backendType].contains(hwAddress))return;
	ILowLevelDeviceBackend *llBackend=0;
	IHighLevelDeviceBackend *hlBackend=0;
	llBackend=createBackendForOutConnection(backendType,hwAddress,connString);
	if(!llBackend)return;
	hlBackend=new StdHighLevelDeviceBackend(llBackend);
	QPointer<IHighLevelDeviceBackend> ptr(hlBackend);
	addDeviceFromBackend(hlBackend);
	if(ptr)
	{
		mOutBackendsByType[hlBackend->backendType()][hlBackend->hwAddress()]=hlBackend;
		connect(hlBackend,&IHighLevelDeviceBackend::destroyedBeforeQObject,this,&Devices::onOutBackendDestroyed);
	}
}

QList<QUuid> Devices::identifiedDevicesIds()
{
	return mIdentifiedDevices.keys();
}

RealDevice* Devices::registerVirtualDevice(VirtualDeviceBackend *be)
{
	if(mVirtualBackends.contains(be->devId()))
	{
		delete be;
		return 0;
	}
	QPointer<VirtualDeviceBackend> ptr(be);
	addDeviceFromBackend(be);
	if(ptr)
	{
		RealDevice *dev=be->device();
		if(!dev||dev->id()!=be->devId())//paranoid mode
			return 0;
		return dev;
	}
	return 0;
}

void Devices::onDeviceMessage(const WLIOT::Message &m)
{
	RealDevice *dev=qobject_cast<RealDevice*>(sender());
	if(!dev||!dev->isReady())
		return;
	if(m.title==WLIOTProtocolDefs::infoMsg)
		qDebug()<<"Device info message ("<<dev->id()<<":"<<dev->name()<<")"<<m.args.join("|");
}

void Devices::onDeviceReIdentified()
{
	RealDevice *dev=(RealDevice*)sender();
	onDeviceReIdentifiedPrivate(dev);
}

void Devices::onDeviceConnected()
{
	RealDevice *dev=(RealDevice*)sender();
	qDebug()<<"Device connected: "<<dev->id()<<":"<<dev->name()<<":"<<
		dev->backend()->backendType()<<":"<<dev->backend()->hwAddress();
	onDeviceConnectedPrivate(dev);
}

void Devices::onDeviceDisconnected()
{
	RealDevice *dev=(RealDevice*)sender();
	qDebug()<<"Device disconnected: "<<dev->id()<<":"<<dev->name()<<":"<<
		dev->backend()->backendType()<<":"<<dev->backend()->hwAddress();
	onDeviceDisconnectedPrivate(dev);
}

void Devices::setupControllers()
{
	checkDevicesFromConfig();
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
	accepted=false;
	QString addr;
	quint16 port;
	TcpDeviceBackend::readAddrFromSocket(s,addr,port);
	TcpDeviceBackend *llBackend=new TcpDeviceBackend(s,addr,port);
	QPointer<StdHighLevelDeviceBackend> hlBackend(new StdHighLevelDeviceBackend(llBackend));
	addDeviceFromBackend(hlBackend.data());
	if(hlBackend)
	{
		mTcpInBackends.insert(hlBackend.data());
		connect(hlBackend.data(),&StdHighLevelDeviceBackend::destroyedBeforeQObject,
			this,&Devices::onTcpInBackendDestroyed);
		accepted=true;
	}
}

void Devices::onNewTcpSslDeviceConnected(qintptr s,bool &accepted)
{
	accepted=false;
	QString addr;
	quint16 port;
	TcpSslDeviceBackend::readAddrFromSocket(s,addr,port);
	TcpSslDeviceBackend *llBackend=new TcpSslDeviceBackend(s,addr,port);
	QPointer<StdHighLevelDeviceBackend> hlBackend(new StdHighLevelDeviceBackend(llBackend));
	addDeviceFromBackend(hlBackend.data());
	if(hlBackend)
	{
		mTcpSslInBackends.insert(hlBackend.data());
		connect(hlBackend.data(),&StdHighLevelDeviceBackend::destroyedBeforeQObject,
			this,&Devices::onTcpSslInBackendDestroyed);
		accepted=true;
	}
}

void Devices::onHubChildDeviceIdentified(const QUuid &deviceId)
{
	RealDevice *dev=(RealDevice*)sender();
	if(!dev)return;
	HubDeviceBackend *chDevBe=dev->childDevice(deviceId);
	if(!chDevBe)return;
	addDeviceFromBackend(chDevBe);
	//TODO if QPointer<chDevBe> is alive ???
}

void Devices::onDevStateChanged(const QByteArrayList &args)
{
	RealDevice *dev=(RealDevice*)sender();
	qDebug()<<"Device state changed: "<<dev->id()<<": "<<args;
	emit deviceStateChanged(dev->id(),args);
}

void Devices::onOutBackendDestroyed()
{
	IHighLevelDeviceBackend *be=(IHighLevelDeviceBackend*)sender();
	mOutBackendsByType[be->backendType()].remove(be->hwAddress());
	checkDevicesFromConfig();
}

void Devices::onTcpInBackendDestroyed()
{
	mTcpInBackends.remove((StdHighLevelDeviceBackend*)sender());
}

void Devices::onTcpSslInBackendDestroyed()
{
	mTcpSslInBackends.remove((StdHighLevelDeviceBackend*)sender());
}

void Devices::onVirtualBackendDestroyed()
{
	mVirtualBackends.remove(((VirtualDeviceBackend*)sender())->devId());
}

void Devices::addDeviceFromBackend(IHighLevelDeviceBackend *hlBackend)
{
	RealDevice *dev=new RealDevice(this);
	dev->setBackend(hlBackend);
	if(!dev->isConnected()||dev->identify()!=RealDevice::OK)
	{
		delete dev;
		return;
	}
	if(mIdentifiedDevices.contains(dev->id()))
	{
		RealDevice *oldDev=mIdentifiedDevices[dev->id()];
		IHighLevelDeviceBackend *oldBe=oldDev->backend();
		if(!oldBe||DeviceTypesPriority::inst().shouldReplace(oldBe->backendType(),hlBackend->backendType()))
			oldDev->setBackend(dev->takeBackend());
		delete dev;
	}
	else
	{
		mIdentifiedDevices[dev->id()]=dev;
		connect(dev,SIGNAL(newMessageFromDevice(WLIOT::Message)),this,SLOT(onDeviceMessage(WLIOT::Message)));
		connect(dev,&RealDevice::connected,this,&Devices::onDeviceConnected,Qt::QueuedConnection);
		connect(dev,&RealDevice::disconnected,this,&Devices::onDeviceDisconnected,Qt::QueuedConnection);
		connect(dev,&RealDevice::childDeviceIdentified,this,&Devices::onHubChildDeviceIdentified);
		connect(dev,SIGNAL(stateChanged(QByteArrayList)),this,SLOT(onDevStateChanged(QByteArrayList)));
		connect(dev,&RealDevice::identified,this,&Devices::onDeviceReIdentified,Qt::QueuedConnection);
		onDeviceReIdentifiedPrivate(dev);
	}
}

ILowLevelDeviceBackend *Devices::createBackendForOutConnection(
	const QByteArray &type,const QString &hwAddress,const QString &connString)
{
	if(type==SerialDeviceBackend::mBackendType)
		return new SerialDeviceBackend(hwAddress,connString,this);
	else if(type==TcpDeviceBackend::mBackendType)
		return new TcpDeviceBackend(hwAddress,connString,this);
	else if(type==TcpSslDeviceBackend::mBackendType)
		return new TcpSslDeviceBackend(hwAddress,connString,this);
	else return 0;
}

QStringList Devices::extractTtyPorts()
{
	QSet<QString> ports;
	QList<QRegExp> portNameRegExps;
	for(QString &pName:MainServerConfig::ttyPortNames)
		portNameRegExps.append(QRegExp(pName,Qt::CaseSensitive,QRegExp::WildcardUnix));
	QMap<QString,LsTtyUsbDevices::DeviceInfo> ttyDevs=LsTtyUsbDevices::allTtyUsbDevices();
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

void Devices::checkDevicesFromConfig()
{
	for(QString &addr:MainServerConfig::tcpAddresses)
	{
		if(addr.isEmpty())
			continue;
		addOutDevice(TcpDeviceBackend::mBackendType,addr,"");
	}
	QStringList ttyPorts=extractTtyPorts();
	qDebug()<<"Found tty devices matching configuration: "<<ttyPorts;
	for(QString &portName:ttyPorts)
	{
		if(portName.isEmpty())
			continue;
		addOutDevice(SerialDeviceBackend::mBackendType,portName,"");
	}
}

void Devices::onDeviceReIdentifiedPrivate(WLIOT::RealDevice *dev)
{
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
	emit deviceIdentified(dev->id(),dev->name(),dev->typeId());
	if(dev->isHubDevice())
		dev->identifyHub();
}

void Devices::onDeviceConnectedPrivate(RealDevice *dev)
{
	emit deviceIdentified(dev->id(),dev->name(),dev->typeId());
}

void Devices::onDeviceDisconnectedPrivate(RealDevice *dev)
{
	emit deviceDisconnected(dev->id());
	dev->setBackend(0);
}

void Devices::terminate()
{
	for(auto &l:QMap<QByteArray,QMap<QString,WLIOT::IHighLevelDeviceBackend*>>(mOutBackendsByType))
		for(auto &b:l)
			b->forceDisconnect();
	mOutBackendsByType.clear();
	for(auto d:QSet<WLIOT::StdHighLevelDeviceBackend*>(mTcpInBackends))
		delete d;
	mTcpInBackends.clear();
	for(auto d:QSet<WLIOT::StdHighLevelDeviceBackend*>(mTcpSslInBackends))
		delete d;
	mTcpSslInBackends.clear();
	for(auto d:QMap<QUuid,WLIOT::VirtualDeviceBackend*>(mVirtualBackends))
		delete d;
	mVirtualBackends.clear();
	for(RealDevice *d:mIdentifiedDevices)
		delete d;
	mIdentifiedDevices.clear();
}
