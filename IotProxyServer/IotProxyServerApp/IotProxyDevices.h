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

#ifndef IOTPROXYDEVICES_H
#define IOTPROXYDEVICES_H

#include "ARpcDevices/ARpcSerialDevice.h"
#include "ARpcDevices/ARpcTcpDevice.h"
#include "ARpcBase/ARpcVirtualDevice.h"
#include "DataCollectionUnit.h"
#include "LsTtyUsbDevices.h"
#include <QObject>
#include "ARpcTcpDeviceDetect.h"

class IotProxyDevices
	:public QObject
{
	Q_OBJECT

public:
	explicit IotProxyDevices(QObject *parent=nullptr);
	void setup();
	QList<QUuid> identifiedDevicesIds();
	ARpcVirtualDevice* registerVirtualDevice(const QUuid &id,const QByteArray &name,
		const QList<ARpcSensorDef> &sensors=QList<ARpcSensorDef>(),
		const ARpcControlsGroup &controls=ARpcControlsGroup());
	ARpcSerialDevice* ttyDeviceByPortName(const QString &portName);
	ARpcTcpDevice* tcpDeviceByAddress(const QString &address);
	ARpcRealDevice* deviceById(const QUuid &id);
	ARpcRealDevice* deviceByIdOrName(const QByteArray &str);
	ARpcVirtualDevice* virtualDeviceByIdOrName(const QByteArray &str);
	bool usbTtyDeviceByPortName(const QString &portName,LsTtyUsbDevices::DeviceInfo &info);
	const QList<ARpcSerialDevice*>& ttyDevices();
	const QList<ARpcTcpDevice*>& tcpDevices();
	const QList<ARpcVirtualDevice*>& virtualDevices();
	const QList<ARpcRealDevice*>& hubDevices();
	ARpcSerialDevice* addTtyDeviceByPortName(const QString &portName);
	ARpcTcpDevice* addTcpDeviceByAddress(const QString &host);
	void terminate();

public slots:
	void setupControllers();

signals:
	void deviceIdentified(QUuid id,QByteArray name,QByteArray type);
	void deviceDisconnected(QUuid id);
	void deviceStateChanged(QUuid id,QByteArrayList args);

private slots:
	void onDeviceMessage(const ARpcMessage &m);
	void onTtyDeviceIdentified();
	void onTcpDeviceIdentified();
	void onVirtualDeviceIdentified();
	void onTtyDeviceDisconnected();
	void onTcpDeviceDisconnected();
	void onNewTcpDeviceConnected(qintptr s,bool &accepted);
	void onHubChildDeviceIdentified(const QUuid &deviceId);
	void onHubChildDeviceLost(const QUuid &deviceId);

private:
	void onDeviceIdentified(ARpcRealDevice *dev);
	void onDeviceDisconnected(ARpcRealDevice *dev);
	QStringList extractTtyPorts();
	ARpcRealDevice* findDeviceByName(const QByteArray &name);

	template<typename T,typename=std::enable_if<std::is_base_of<ARpcRealDevice,T>::value>>
	ARpcRealDevice* findDevById(const QUuid &id,QList<T*> &list)
	{
		static_assert(std::is_base_of<ARpcRealDevice,T>::value,"Invalid template argument");
		for(ARpcRealDevice *d:list)
			if(d->id()==id)
				return d;
		return 0;
	}

private:
	QList<ARpcSerialDevice*> mTtyDevices;
	QList<ARpcTcpDevice*> mTcpDevices;
	QList<ARpcVirtualDevice*> mVirtualDevices;
	QList<ARpcRealDevice*> mHubDevices;
	QMap<QUuid,ARpcRealDevice*> identifiedDevices;
	QList<LsTtyUsbDevices::DeviceInfo> allTtyUsbDevices;
	QFileSystemWatcher watcher;
	ARpcTcpDeviceDetect tcpServer;
};

#endif // IOTPROXYDEVICES_H
