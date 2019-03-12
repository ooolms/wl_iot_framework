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

#include "wliot/devices/SerialDevice.h"
#include "wliot/devices/TcpDevice.h"
#include "wliot/devices/VirtualDevice.h"
#include "DataCollectionUnit.h"
#include "LsTtyUsbDevices.h"
#include <QObject>
#include "TcpDeviceDetect.h"

class Devices
	:public QObject
{
	Q_OBJECT

public:
	explicit Devices(QObject *parent=nullptr);
	void setup();
	QList<QUuid> identifiedDevicesIds();
	VirtualDevice* registerVirtualDevice(const QUuid &id,const QByteArray &name);
	SerialDevice* ttyDeviceByPortName(const QString &portName);
	TcpDevice* tcpDeviceByAddress(const QString &address);
	RealDevice* deviceById(const QUuid &id);
	RealDevice* deviceByIdOrName(const QByteArray &str);
	VirtualDevice* virtualDeviceByIdOrName(const QByteArray &str);
	bool usbTtyDeviceByPortName(const QString &portName,LsTtyUsbDevices::DeviceInfo &info);
	const QList<SerialDevice*>& ttyDevices();
	const QList<TcpDevice*>& tcpDevices();
	const QList<VirtualDevice*>& virtualDevices();
	const QList<RealDevice*>& hubDevices();
	SerialDevice* addTtyDeviceByPortName(const QString &portName);
	TcpDevice* addTcpDeviceByAddress(const QString &host);
	void terminate();

public slots:
	void setupControllers();

signals:
	void deviceIdentified(QUuid id,QByteArray name,QByteArray type);
	void deviceDisconnected(QUuid id);
	void deviceStateChanged(QUuid id,QByteArrayList args);

private slots:
	void onDeviceMessage(const Message &m);
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
	void onDeviceIdentified(RealDevice *dev);
	void onDeviceDisconnected(RealDevice *dev);
	QStringList extractTtyPorts();
	RealDevice* findDeviceByName(const QByteArray &name);

	template<typename T,typename=std::enable_if<std::is_base_of<RealDevice,T>::value>>
	RealDevice* findDevById(const QUuid &id,QList<T*> &list)
	{
		static_assert(std::is_base_of<RealDevice,T>::value,"Invalid template argument");
		for(RealDevice *d:list)
			if(d->id()==id)
				return d;
		return 0;
	}

private:
	QList<SerialDevice*> mTtyDevices;
	QList<TcpDevice*> mTcpDevices;
	QList<VirtualDevice*> mVirtualDevices;
	QList<RealDevice*> mHubDevices;
	QMap<QUuid,RealDevice*> identifiedDevices;
	QList<LsTtyUsbDevices::DeviceInfo> allTtyUsbDevices;
	QFileSystemWatcher watcher;
	TcpDeviceDetect tcpServer;
};

#endif // DEVICES_H
