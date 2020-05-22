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

#ifndef DEVICES_H
#define DEVICES_H

#include "wliot/devices/SerialDeviceBackend.h"
#include "wliot/devices/TcpDeviceBackend.h"
#include "wliot/devices/VirtualDevice.h"
#include "DataCollectionUnit.h"
#include "LsTtyUsbDevices.h"
//#include <libusb.h>
#include <QObject>
#include "TcpDeviceDetect.h"

class Devices
	:public QObject
{
	Q_OBJECT

public:
	explicit Devices(QObject *parent=nullptr);
	virtual ~Devices();
	void setup();
	QList<QUuid> identifiedDevicesIds();
	WLIOT::VirtualDevice* registerVirtualDevice(const QUuid &id,const QByteArray &name,const QUuid &typeId);
	WLIOT::RealDevice *ttyDeviceByPortName(const QString &portName);
	WLIOT::RealDevice* tcpDeviceByAddress(const QString &address);
	WLIOT::RealDevice* deviceById(const QUuid &id);
	WLIOT::RealDevice* deviceByIdOrName(const QByteArray &idOrName);
	bool usbTtyDeviceByPortName(const QString &portName,LsTtyUsbDevices::DeviceInfo &info);
	WLIOT::RealDevice* addTtyDeviceByPortName(const QString &portName);
	WLIOT::RealDevice* addTcpDeviceByAddress(const QString &host);
	void terminate();

public slots:
	void setupControllers();

signals:
	void deviceIdentified(QUuid id,QByteArray name,QUuid typeId);
	void deviceDisconnected(QUuid id);
	void deviceStateChanged(QUuid id,QByteArrayList args);

private slots:
	void onDeviceMessage(const WLIOT::Message &m);
	void onTtyDeviceIdentified();
	void onTcpDeviceIdentified();
	void onVirtualDeviceIdentified();
	void onTtyDeviceDisconnected();
	void onTcpDeviceDisconnected();
	void onVirtualDeviceDisconnected();
	void onNewTcpDeviceConnected(qintptr s,bool &accepted);
	void onHubChildDeviceIdentified(const QUuid &deviceId);
	void onHubChildDeviceLost(const QUuid &deviceId);
	void onDevStateChanged(const QByteArrayList &args);

private:
	void onDeviceIdentified(WLIOT::RealDevice *dev);
	void onDeviceDisconnected(WLIOT::RealDevice *dev);
	QStringList extractTtyPorts();
//	static int onUsbDeviceEventStatic(libusb_context *ctx,
//		libusb_device *device,libusb_hotplug_event event,void *user_data);
//	int onUsbDeviceAttached(libusb_device *device);
//	int onUsbDeviceDetached(libusb_device *device);

	template<typename T,typename=std::enable_if<std::is_base_of<WLIOT::RealDevice,T>::value>>
	WLIOT::RealDevice* findDevById(const QUuid &id,QList<T*> &list)
	{
		static_assert(std::is_base_of<WLIOT::RealDevice,T>::value,"Invalid template argument");
		for(WLIOT::RealDevice *d:list)
			if(d->id()==id)
				return d;
		return nullptr;
	}

private:
	QList<WLIOT::RealDevice*> mTtyDevices;
	QList<WLIOT::RealDevice*> mTcpInDevices;
	QList<WLIOT::RealDevice*> mTcpOutDevices;
	QList<WLIOT::VirtualDevice*> mVirtualDevices;
	QList<WLIOT::HubDevice*> mHubDevices;
	QMap<QUuid,WLIOT::RealDevice*> identifiedDevices;
	QList<LsTtyUsbDevices::DeviceInfo> allTtyUsbDevices;
	QFileSystemWatcher watcher;
	TcpDeviceDetect tcpServer;
//	libusb_hotplug_callback_handle usbCbHandle;
};

#endif // DEVICES_H
