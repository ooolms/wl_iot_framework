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

#include "DataCollectionUnit.h"
#include "LsTtyUsbDevices.h"
#include "wliot/devices/StdHighLevelDeviceBackend.h"
#include "wliot/devices/VirtualDeviceBackend.h"
#include <QFileSystemWatcher>
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

	/**
	 * @brief registerVirtualDevice
	 * destroyes backend if failed
	 * @param be
	 * @return
	 */
	WLIOT::RealDevice* registerVirtualDevice(WLIOT::VirtualDeviceBackend *be);
	WLIOT::IHighLevelDeviceBackend* ttyBackendByPortName(const QString &portName);
	WLIOT::IHighLevelDeviceBackend* tcpBackendByAddress(const QString &address);
	WLIOT::IHighLevelDeviceBackend* tcpSslBackendByAddress(const QString &address);
	WLIOT::RealDevice* deviceById(const QUuid &id);
	WLIOT::RealDevice* deviceByIdOrName(const QByteArray &idOrName);
	bool usbTtyDeviceByPortName(const QString &portName,LsTtyUsbDevices::DeviceInfo &info);
	void addOutDevice(const QByteArray &backendType,const QString &hwAddress,const QString &connString);
	void terminate();

public slots:
	void setupControllers();

signals:
	void deviceIdentified(QUuid id,QByteArray name,QUuid typeId);
	void deviceDisconnected(QUuid id);
	void deviceStateChanged(QUuid id,QByteArrayList args);

private slots:
	void onDeviceMessage(const WLIOT::Message &m);
	void onDeviceReIdentified();
	void onDeviceConnected();
	void onDeviceDisconnected();
	void onNewTcpDeviceConnected(qintptr s,bool &accepted);
	void onNewTcpSslDeviceConnected(qintptr s,bool &accepted);
	void onHubChildDeviceIdentified(const QUuid &deviceId);
	void onDevStateChanged(const QByteArrayList &args);
	void onOutBackendDestroyed();
	void onTcpInBackendDestroyed();
	void onTcpSslInBackendDestroyed();
	void onVirtualBackendDestroyed();

private:
	void onDeviceReIdentifiedPrivate(WLIOT::RealDevice *dev);
	void onDeviceConnectedPrivate(WLIOT::RealDevice *dev);
	void onDeviceDisconnectedPrivate(WLIOT::RealDevice *dev);

	/**
	 * @brief addDeviceFromBackend
	 * Подключает устройство и идентифицирует, потом проверяет, нет ли уже с таким же id.
	 * Если есть, проверяет приоритет backend-ов. Если у нового меньше, грохает его.
	 * Так что перед вызовом этой функции нужно завернуть backend в QPointer, а потом проверить,
	 * прежде чем добавлять в какой-либо список backend-ов
	 * @param hlBackend
	 */
	void addDeviceFromBackend(WLIOT::IHighLevelDeviceBackend *hlBackend);
	WLIOT::ILowLevelDeviceBackend* createBackendForOutConnection(
		const QByteArray &type,const QString &hwAddress,const QString &connString);
	QStringList extractTtyPorts();
	void checkDevicesFromConfig();
//	static int onUsbDeviceEventStatic(libusb_context *ctx,
//		libusb_device *device,libusb_hotplug_event event,void *user_data);
//	int onUsbDeviceAttached(libusb_device *device);
//	int onUsbDeviceDetached(libusb_device *device);

private:
	QMap<QByteArray,QMap<QString,WLIOT::IHighLevelDeviceBackend*>> mOutBackendsByType;
	QSet<WLIOT::StdHighLevelDeviceBackend*> mTcpInBackends;//входящие tcp соединения
	QSet<WLIOT::StdHighLevelDeviceBackend*> mTcpSslInBackends;//входящие tcp+ssl соединения
	QMap<QUuid,WLIOT::VirtualDeviceBackend*> mVirtualBackends;
	QMap<QUuid,WLIOT::RealDevice*> mIdentifiedDevices;
	QMap<QString,LsTtyUsbDevices::DeviceInfo> allTtyUsbDeviceInfos;
	QFileSystemWatcher watcher;
	TcpDeviceDetect tcpServer;
//	libusb_hotplug_callback_handle usbCbHandle;
};

#endif // DEVICES_H
